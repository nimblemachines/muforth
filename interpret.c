/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Interpreter and compiler */

#include "muforth.h"

#include <stdlib.h>
#include <ctype.h>

/* for debug */
#include <sys/uio.h>
#include <unistd.h>

#ifdef DEBUG_STACK
#include <stdio.h>
#endif

#ifdef DEBUG_TOKEN
#include <stdio.h>
#endif

struct imode        /* interpreter mode */
{
    xtk eat;       /* consume one token */
    xtk prompt;    /* display a mode-specific prompt */
};

/*
 * In the parsing code that follows, the variables first and last are
 * negative offsets from the _end_ of the input text. They increase from
 * left to right, and are always non-positive (ie, <= 0).
 *
 * Because they increase from left to right, just like "normal" text
 * pointers, a "last - first" difference yields a non-negative (>=0)
 * length.
 */
static struct text source;
static ssize_t first;       /* goes from -start to 0 */

struct string parsed;       /* for errors */

static void mu_return_token(ssize_t last, int trailing)
{
    /* Get address and length of the token */
    parsed.data = source.end + first;
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
    ssize_t last;

    DUP;   /* we'll be setting TOP when we're done */

    /* Skip leading whitespace */
    for (; first != 0 && isspace(source.end[first]); first++)
        ;

    /*
     * Scan for trailing whitespace and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last != 0; last++)
        if (isspace(source.end[last]))
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
    ssize_t last;

    /* The first character of unseen input is the first character of token. */

    /*
     * Scan for trailing delimiter and consume it, unless we run out of
     * input text first.
     */
    for (last = first; last != 0; last++)
        if (TOP == source.end[last])
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

static pw p_mu_complain = &mu_complain;

void mu_huh_q()
{
    if (POP) return;
    mu_complain();
}

static xtk mu_number = XTK(mu_complain);
static xtk mu_number_comma = XTK(mu_complain);

void mu_push_tick_number()
{
    PUSH(&mu_number);
}

void mu_push_tick_number_comma()
{
    PUSH(&mu_number_comma);
}

void mu_execute() { EXECUTE; }

/* The interpreter's "consume" function. */
void mu__lbracket()
{
    mu_push_forth_chain();
    mu_find();
    if (POP)
    {
        EXECUTE;
        return;
    }
    execute(mu_number);
}

/* The compiler's "consume" function. */
void mu__rbracket()
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
    execute(mu_number_comma);
}

void mu_nope() {}       /* very useful NO-OP */
void mu_zzz() {}        /* a convenient GDB breakpoint */

/*
 * Remember that the second part of a struct imode is a pointer to code to
 * print a mode-specific prompt. The muforth kernel lacks decent I/O
 * facilities. Until these are defined in startup.mu4, the prompts are
 * noops.
 */
static pw p_mu__lbracket = &mu__lbracket;
static pw p_mu__rbracket = &mu__rbracket;
pw p_mu_nope = &mu_nope;

static struct imode forth_interpreter  = { XTK(mu__lbracket), XTK(mu_nope) };
static struct imode forth_compiler     = { XTK(mu__rbracket), XTK(mu_nope) };

static struct imode *state = &forth_interpreter;

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
    source.end = (char *) ST1 + TOP; /* the _end_ of the text */
    source.start = -TOP;        /* offset to the start of text */
    DROP(2);

    first = source.start;

    for (;;)
    {
        mu_token();
        if (TOP == 0) break;
        execute(state->eat);   /* consume(); */
        mu_qstack();
    }
    DROP(2);
}

static pw p_mu_interpret = &mu_interpret;

void mu_evaluate()
{
    struct text saved_source;
    ssize_t saved_first;

    saved_source = source;
    saved_first = first;

    PUSH(XTK(mu_interpret));
    mu_catch();
    source = saved_source;
    first = saved_first;
    mu_throw();
}

static pw p_mu_evaluate = &mu_evaluate;

void mu_load_file()
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

void mu_start_up()
{
    PUSH("warm");       /* push the token "warm" */
    PUSH(4);
    mu__lbracket();     /* ... and execute it! */
}

void mu_bye()
{
    exit(0);
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

