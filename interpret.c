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

static struct text source;
char *first;                /* goes from source.start to source.end */

static int lineno = 1;      /* line number - incremented for each newline */
int token_lineno;           /* captured with first character of token */
char *token_first;          /* first char of token - captured for die */

/* We have to be able to run thru all of startup.mu4 using the interpreter
 * and compiler defined here. Thus, we need to "defer" access to
 * interpreting and compiling -numbers-. The number input code in startup
 * will patch into these variables but leave the token consumers alone.
 * Only after we've loaded startup and returned to Forth do we fire up the
 * "new" interpret loop.
 */
static xtk tick_number          = XTK(mu_nope);
static xtk tick_number_comma    = XTK(mu_nope);

void mu_push_tick_number()          { PUSH(&tick_number); }
void mu_push_tick_number_comma()    { PUSH(&tick_number_comma); }

/* Push captured line number */
void mu_at_line() { PUSH(token_lineno); }

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
    NIP(-1);    /* make room for result */

    /* Get address and length of the token */
    ST1 = (cell) first;
    TOP = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

#ifdef DEBUG_TOKEN
    fprintf(stderr, "%.*s\n", TOP, ST1);
#endif
}

void mu_token()  /* -- start len */
{
    char *last;

    DUP;   /* we'll be setting TOP when we're done */

    /* Skip leading whitespace */
    while (first < source.end && isspace(*first))
    {
        if (*first == '\n') lineno++;
        first++;
    }

    /* capture lineno that token begins on */
    token_lineno = lineno;

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

    /* The first character of unseen input is the first character of token. */

    /* capture lineno that token begins on */
    token_lineno = lineno;

    /*
     * Scan for trailing delimiter and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last < source.end; last++)
    {
        if (*last == '\n') lineno++;
        if (TOP == *last)
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

/* The interpreter's "consume" function. */
void mu_interpret_token()
{
    mu_push_forth_chain();
    mu_find();
    if (POP)
    {
        mu_execute();
        return;
    }
    mu_push_tick_number();
    mu_fetch();
    mu_execute();
}

/* The compiler's "consume" function. */
void mu_compile_token()
{
    mu_push_compiler_chain();
    mu_find();
    if (POP)
    {
        mu_execute();
        return;
    }
    mu_push_forth_chain();
    mu_find();
    if (POP)
    {
        mu_compile_comma();
        return;
    }
    mu_push_tick_number_comma();
    mu_fetch();
    mu_execute();
}

static void (*eat)() = &mu_interpret_token;

void mu_compiler_lbracket()
{
    eat = &mu_interpret_token;
}

void mu_minus_rbracket()
{
    eat = &mu_compile_token;
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

/*
 * This version of interpret is -not- exported to Forth! We are going to
 * re-define it in Forth so it executes as "pure" Forth, and we can use
 * Forth-side, return-stack-based exception handling!
 */
static void mu_interpret()
{
    source.start = (char *)ST1;
    source.end   = (char *)ST1 + TOP;
    DROP(2);

    first = source.start;

    for (;;)
    {
        mu_token();
        if (TOP == 0) break;
        token_first = (char *)ST1;  /* capture for die() */
        (*eat)();           /* eat is a C function pointer! */
        mu_qstack();
    }
    DROP(2);
}

void _mu_load_file()    /* c-string-name */
{
    int fd;

    mu_push_r_slash_o();
    mu_open_file();
    fd = TOP;
    mu_mmap_file();

    lineno = 1;

    mu_interpret();
    close(fd);
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

