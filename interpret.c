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

#if defined (DEBUG_STACK) || defined(DEBUG_TOKEN)
#include <stdio.h>
#endif

static struct text source;
static char *first;         /* goes from source.start to source.end */

static int lineno = 1;      /* line number - incremented for each newline */
int parsed_lineno;          /* captured from lineno at start of token/parse */
struct string parsed;       /* for errors */

/* Push address of lineno variable */
void mu_push_line() { PUSH(&lineno); }

/*
 * This isn't exactly ANS-kosher, since traditionally >IN contained an
 * offset within the input text that went from 0 to length-1; here it goes
 * from source.start to source.end-1, but the important effect is that we
 * can do this:
 *
 * >in @ <parse> >in ! <re-parse>
 */
void mu_first()
{
    PUSH(&first);
}

void mu_source()
{
    PUSH(&source);
}

static void mu_return_token(char *last, int trailing)
{
    /* Get address and length of the token */
    parsed.data = first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    DROP(-2);
    ST1 = (cell) parsed.data;
    TOP = parsed.length;

#ifdef DEBUG_TOKEN
    fprintf(stderr, "%.*s\n", TOP, ST1);
#endif
}

/* Skip leading whitespace */
static void skip()
{
    while (first < source.end && isspace(*first))
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

    for (last = first; last < source.end; last++)
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
        mu_compile_comma();
        return;
    }
    mu_complain();
}

static void (*eat)() = &muboot_interpret_token;

/* XXX: should these two be deferred? */
void mu_compiler_lbracket()
{
    eat = &muboot_interpret_token;
}

void mu_minus_rbracket()
{
    eat = &muboot_compile_token;
}

void mu_push_parsed()
{
    PUSH((cell) parsed.data);
    PUSH(parsed.length);
}

void mu_qstack()
{
    if (SP > S0)
    {
        mu_sp_reset();
        abort_zmsg("tried to pop an empty stack");
    }
    if (SP < SMAX)
    {
        abort_zmsg("too many items on the stack");
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

/*
 * This version of interpret is -not- exported to Forth! We are going to
 * re-define it in Forth so it executes as "pure" Forth, and we can use
 * Forth-side, return-stack-based exception handling!
 */
static void muboot_interpret()
{
    source.start = (char *)ST1;
    source.end   = (char *)ST1 + TOP;
    DROP(2);

    first = source.start;

    for (;;)
    {
        mu_token();
        if (TOP == 0) break;
        (*eat)();           /* eat is a C function pointer! */
        mu_qstack();
    }
    DROP(2);
}

void muboot_load_file()    /* c-string-name */
{
    int fd;

    zloading = (char *)TOP;     /* capture file name for errors */
    mu_open_file_ro();
    fd = TOP;
    mu_read_file();

    lineno = 1;

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

