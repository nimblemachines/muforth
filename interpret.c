/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
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

char *zloading;             /* the file we're currently loading; C string */
static int lineno = 1;      /* line number - incremented for each newline */
static int parsed_lineno;   /* captured with first character of token */
struct string parsed;       /* for errors */

/* Push captured line number */
void mu_line()
{
    PUSH(parsed_lineno);
}

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

    PUSH((cell) parsed.data);
    PUSH(parsed.length);

#ifdef DEBUG_TOKEN
    fprintf(stderr, "%.*s\n", parsed.length, parsed.data);
#endif
}

void mu_token()  /* -- start len */
{
    char *last;

    /* Skip leading whitespace */
    while (first < source.end && isspace(*first))
    {
        if (*first == '\n') lineno++;
        first++;
    }

    /* capture lineno that token begins on */
    parsed_lineno = lineno;

    /*
     * Scan for trailing whitespace and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last < source.end; last++)
        if (isspace(*last))
        {
            if (*last == '\n') lineno++;

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
    char delim = POP;

    /* The first character of unseen input is the first character of token. */

    /* capture lineno that token begins on */
    parsed_lineno = lineno;

    /*
     * Scan for trailing delimiter and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last < source.end; last++)
    {
        if (*last == '\n') lineno++;
        if (delim == *last)
        {
            /* found trailing delimiter; consume it */
            mu_return_token(last, 1);
            return;
        }
    }

    /* ran out of text; don't consume trailing */
    mu_return_token(last, 0);
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
    throw("isn't defined");
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

void mu_minus_rbracket()
{
    state = &forth_compiler;
}

void mu_push_parsed()
{
    PUSH((cell) parsed.data);
    PUSH(parsed.length);
}

static void mu_qstack()
{
    if (SP > S0)
    {
        mu_sp_reset();
        throw("tried to pop an empty stack");
    }
    if (SP < SMAX)
    {
        throw("too many items on the stack");
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
    int saved_lineno = lineno;
    char *saved_zloading = zloading;
    char *newfile = (char *)TOP;

    mufs_push_r_slash_o();
    mufs_open_file();
    fd = TOP;
    mufs_mmap_file();

    /* wait to reset these until just before we evaluate the new file */
    lineno = 1;
    zloading = newfile;

    PUSH(XTK(mu_evaluate));
    mu_catch();
    lineno = saved_lineno;
    zloading = saved_zloading;
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

