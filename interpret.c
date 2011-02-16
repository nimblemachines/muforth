/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* Interpreter and compiler */

#include "muforth.h"

#include <ctype.h>

/* for debug */
#include <sys/uio.h>

#if defined (DEBUG_STACK) || defined(DEBUG_TOKEN)
#include <stdio.h>
#endif

/* interpreter mode */
struct imode
{
    xtk eat;        /* consume one token */
    xtk prompt;     /* display a mode-specific prompt */
};

static char *start;    /* input source text */
static char *end;
static char *first;    /* goes from start to end */

/* line number - incremented for each newline */
static cell lineno = 1;

int parsed_lineno;          /* captured with first character of token */
struct string parsed;       /* for errors */

/* Push lineno variable */
void mu_push_line()
{
    PUSH_ADDR(&lineno);
}

/* Push captured line number */
void mu_at_line()
{
    PUSH(parsed_lineno);
}

void mu_push_first()
{
    PUSH_ADDR(&first);
}

void mu_push_start()
{
    PUSH_ADDR(&start);
}

void mu_push_end()
{
    PUSH_ADDR(&end);
}

static void mu_return_token(char *last, int trailing)
{
    /* Get address and length of the token */
    parsed.data = first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    DROP(-2);
    ST1 = (addr) parsed.data;
    TOP = parsed.length;

#ifdef DEBUG_TOKEN
    /* Without these casts, this doesn't work! */
    fprintf(stderr, "%.*s\n", (int)TOP, (char *)ST1);
#endif
}

/* Skip leading whitespace */
static void skip()
{
    while (first < end && isspace(*first))
    {
        if (*first == '\n') lineno++;
        first++;
    }
}

/*
 * Scan for trailing delimiter and consume it, unless we run out of
 * input text first.
 */
static void mu_scan(int delim)
{
    char *last;

    /* capture lineno that token begins on */
    parsed_lineno = lineno;

    for (last = first; last < end; last++)
    {
        if (*last == '\n') lineno++;
        if (delim == *last
            || (delim == ' ' && isspace(*last)))
        {
            /* found trailing delimiter; consume it */
            mu_return_token(last, 1);
            return;
        }
    }

    /* ran out of text; don't consume trailing */
    mu_return_token(last, 0);
}

void mu_token()  /* -- start len */
{
    skip();         /* skip leading whitespace */
    mu_scan(' ');   /* scan for trailing whitespace and collect token */
}

void mu_parse()  /* delim -- start len */
{
    /* The first character of unseen input is the first character of token. */
    mu_scan(POP);   /* scan for trailing delimiter and collect token */
}

/*
: complain   error"  is not defined"  -;
: huh?   if ^ then complain  ;   ( useful after find or token' )
defer not-defined  now complain is not-defined
*/

/* We're going to assume that in every case that complain is called, that
 * there is a token ( a u) on the stack. So complain will do a 2drop.
 */
void mu_complain()
{
    DROP(2);
    abort_zmsg("isn't defined");
}

void mu_huh_q()
{
    if (POP) return;
    mu_complain();
}

/* The interpreter's "consume" function. */
/* Not declared "static" because it is needed by muforth.c to fire up warm! */
void muboot_interpret_token()
{
    mu_push_forth_chain();
    mu_find();
    if (POP)
    {
        EXECUTE;
        return;
    }
    mu_complain();
}

/* The compiler's "consume" function. */
static void muboot_compile_token()
{
    mu_push_compiler_chain();
    mu_find();
    if (POP)
    {
        EXECUTE;
        return;
    }
    mu_push_forth_chain();
    mu_find();
    if (POP)
    {
        mu_comma();
        return;
    }
    mu_complain();
}

/*
 * Remember that the second part of a struct imode is a pointer to code to
 * print a mode-specific prompt. The muforth kernel lacks decent I/O
 * facilities. Until these are defined in startup.mu4, the prompts are
 * noops.
 */
CODE(mu_nope)
CODE(muboot_interpret_token)
CODE(muboot_compile_token)

static struct imode forth_interpreter =
    { XTK(muboot_interpret_token), XTK(mu_nope) };

static struct imode forth_compiler =
    { XTK(muboot_compile_token),   XTK(mu_nope) };

static struct imode *state = &forth_interpreter;


void mu_consume()
{
    /* call the current consume function */
    execute_xtk(state->eat);  
}

void mu_push_state()
{
    PUSH_ADDR(&state);
}

void mu_compiler_lbracket()
{
    state = &forth_interpreter;
}

void mu_minus_rbracket()
{
    state = &forth_compiler;
}

void mu_push_parsed()
{
    PUSH((addr) parsed.data);
    PUSH(parsed.length);
}

static xtk xtk_show_stack = XTK(mu_nope);
void mu_push_tick_show_stack()  { PUSH_ADDR(&xtk_show_stack); }

void mu_qstack()
{
    if (SP > S0)
    {
        mu_sp_reset();
        return abort_zmsg("tried to pop an empty stack");
    }
    if (SP < SMAX)
    {
        return abort_zmsg("too many items on the stack");
    }

    /* Call Forth code to print stack, maybe. */
    execute_xtk(xtk_show_stack);

#ifdef DEBUG_STACK
    {
        int i;
        fprintf(stderr, "  --");
        for (i = 0; i < 4; i++)
            fprintf(stderr, "  %16llx", (uval)SP[3-i]);
        fprintf(stderr, "\n");
        fflush(stderr);
    }
#endif
}

/*
 * This version of interpret is -not- exported to Forth! We are going to
 * re-define it in Forth so it executes as "pure" Forth, and we can use
 * Forth-side, return-stack-based exception handling!
 *
 * Oddly enough, the Forth implementation will be be *exactly* the same!
 * The only difference is that it will be executing in Forth, so we can use
 * R stack tricks to change its behaviour.
 */
static void muboot_interpret()
{
    for (;;)
    {
        mu_token();
        if (TOP == 0) break;
        mu_consume();
        mu_qstack();
    }
    DROP(2);
}

/*
 * This will also be re-implemented in Forth, but with nice nesting of
 * various context variables: radix, first, start/end, etc.
 */
void muboot_load_file()    /* c-string-name */
{
    int fd;

    mu_open_file_ro();
    fd = TOP;
    mu_read_file();

    start = (char *)ST1;
    end   = (char *)ST1 + TOP;
    DROP(2);

    /* wait to reset these until just before we evaluate the new file */
    lineno = 1;
    first = start;

    muboot_interpret();

    PUSH(fd);
    mu_close_file();
}

/*
: consume   state @  @execute ;

: _[   ( interpret one token)
      .forth. find  if execute ^ then  number ;

: _]   ( compile one token)
   .compiler. find  if  execute  ^ then
      .forth. find  if  compile, ^ then    number, ;

-:  compiler -"find if  assembler -"find if  number, exit  then
        compile, exit  then  execute ;

-:  assembler -"find if  outside -"find if  target -"find  if
      ( not found)  number exit  then
      ( target)  @  \o >DATA  ( target pfa contents)  exit  then  then
      execute  ;

-:  outside -"find  if  target -"find  if  tnumber drop  exit  then
      @ ( target pfa)  remote exit  then  execute  ;

-:  inside -"find  if   target -"find  if  tnumber, exit  then
      @ ( target pfa)  \o ,  exit  then   execute  ;

-:  definer -"find  if  compiler -"find  if  ( execute if either of these)
    outside -"find  if     forth -"find  if  number, exit  then  then
        compile, exit  then  then   execute  ;

( stack underflow)
: ?stack   depth 0< if  error"  ate the stack"  then  ;

: interpret ( a u)
   source 2!  0 >in !
   begin  token  dup  while  consume  ?stack  repeat  2drop  ;

: evaluate  ( a u)
   source 2@ 2push  >in @ push  lit interpret catch
   pop >in !  2pop source 2!  throw  ;

compiler definitions
: -;     \ [  ;             ( for words that don't end with ^)
: ^   lit unnest compile,  ;   ( right now this doesn't do anything fancy)
: ;      \ ^  \ -;  ;
forth definitions
*/

