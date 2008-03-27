/*
 * This file is part of muFORTH: http://pages.nimblemachines.com/muforth
 *
 * Copyright (c) 2002-2008 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* Interpreter and compiler */

#include "muforth.h"

#include <ctype.h>

/* for debug */
#include <sys/uio.h>
#include <unistd.h>

#if defined (DEBUG_STACK) || defined(DEBUG_TOKEN)
#include <stdio.h>
#endif

struct imode        /* interpreter mode */
{
    xtk eat;        /* consume one token */
    xtk prompt;     /* display a mode-specific prompt */
};

static struct text source;
static char *first;         /* goes from source.start to source.end */
struct string parsed;       /* for errors */

/*
 * This isn't exactly ANS-kosher, since traditionally >IN contained an
 * offset within the input text that went from 0 to length-1; here it goes
 * from source.start to source.end-1, but the important effect is that we
 * can do this:
 *
 * >in @ <parse> >in ! <re-parse>
 */
void mu_to_in()
{
    PUSH(&first);
}

static void mu_return_token(char *last, int trailing)
{
    /* Get address and length of the token */
    parsed.data = first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    NIP(-1);    /* make room for result */
    ST1 = (cell) parsed.data;
    TOP = parsed.length;

#ifdef DEBUG_TOKEN
    fprintf(stderr, "%.*s\n", parsed.length, parsed.data);
#endif
}

void mu_token()  /* -- start len */
{
    char *last;

    DUP;   /* we'll be setting TOP when we're done */

    /* Skip leading whitespace */
    for (; first < source.end && isspace(*first); first++)
        ;

    /*
     * Scan for trailing whitespace and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last < source.end; last++)
        if (isspace(*last))
        {
            /* found trailing whitespace; consume it */
            mu_return_token(last, 1);
            return;
        }

    /* ran out of text; don't consume trailing */
    mu_return_token(last, 0);
}

void mu_parse()  /* delim -- start len */
{
    char *last;

    /* The first character of unseen input is the first character of token. */

    /*
     * Scan for trailing delimiter and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last < source.end; last++)
        if (TOP == *last)
        {
            /* found trailing delimiter; consume it */
            mu_return_token(last, 1);
            return;
        }

    /* ran out of text; don't consume trailing */
    mu_return_token(last, 0);
}

/*
: complain   error"  is not defined"  -;
: huh?   if ^ then complain  ;   ( useful after find or token' )
defer not-defined  now complain is not-defined
*/

void mu_complain()
{
    PUSH(isnt_defined);
    mu_throw();
}

void mu_huh_q()
{
    if (POP) return;
    mu_complain();
}

void mu_execute() { EXECUTE; }

/* The interpreter's "consume" function. */
void _mu__lbracket()
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
void _mu__rbracket()
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
        mu_compile_comma();
        return;
    }
    mu_complain();
}

void mu_nope() {}    /* very useful NO-OP */
void mu_zzz()  {}    /* a convenient GDB breakpoint */

/*
 * Remember that the second part of a struct imode is a pointer to code to
 * print a mode-specific prompt. The muforth kernel lacks decent I/O
 * facilities. Until these are defined in startup.mu4, the prompts are
 * noops.
 */

static struct imode forth_interpreter  = { XTK(_mu__lbracket), XTK(mu_nope) };
static struct imode forth_compiler     = { XTK(_mu__rbracket), XTK(mu_nope) };

static struct imode *state = &forth_interpreter;


static void consume()
{
    execute_xtk(state->eat);      /* call the current consume function */
}

void mu_push_state()
{
    PUSH(&state);
}

void mu_compiler_lbracket()
{
    state = &forth_interpreter;
}

/* to make it easier to compile into ; */
void mu_backslash_lbracket()
{
    state = &forth_interpreter;
}

void mu_minus_rbracket()
{
    state = &forth_compiler;
}

void mu_push_parsed()
{
    DUP; NIP(-1);
    ST1 = (cell) parsed.data;
    TOP = parsed.length;
}

static void mu_qstack()
{
    if (SP > S0)
    {
        PUSH(ate_the_stack);
        mu_throw();
    }
#ifdef DEBUG_STACK
    /* print stack */
    {
        cell *p;

        printf("  [ ");

        for (p = S0; p > SP; )
            printf("%x ", *--p);

        printf("] %x\n", TOP);
    }
#endif
}

void mu_interpret()
{
    source.start = (char *)ST1;
    source.end =   (char *)ST1 + TOP;
    DROP(2);

    first = source.start;

    for (;;)
    {
        mu_token();
        if (TOP == 0) break;
        consume();
        mu_qstack();
    }
    DROP(2);
}

void mu_evaluate()
{
    struct text saved_source;
    char *saved_first;

    saved_source = source;
    saved_first = first;

    PUSH(XTK(mu_interpret));
    mu_catch();
    source = saved_source;
    first = saved_first;
    mu_throw();
}

void mu_load_file()    /* c-string-name */
{
    int fd;

    mu_push_r_slash_o();
    mu_open_file();
    fd = TOP;
    mu_mmap_file();
    PUSH(XTK(mu_evaluate));
    mu_catch();
    close(fd);
    mu_throw();
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

