/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
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

struct imode			/* interpreter mode */
{
    void (*eat)();		/* consume one token */
    void (*prompt)();		/* display a mode-specific prompt */
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
static ssize_t first;		/* goes from -start to 0 */

struct string parsed;		/* for errors */

#if 0
/* Parse whitespace-delimited tokens */
void mu_token_old()
{
    ssize_t last;
    int trailing;

    /* regardless of the outcome, we return (start, length) */
    DROP(-2);

    if (first == 0)
    {
	STK(1) = TOP = 0;
	return;
    }
	
    /* Skip leading whitespace */
    for (;;)
    {
	if (!isspace(source.end[first])) break;
	if (++first == 0)
	{
	    STK(1) = TOP = 0;
	    return;
	}
    }

    /*
     * Now scan for trailing delimiter and consume it,
     * unless we run out of input text first.
     */
    last = first;
    trailing = 1;
    for (;;)
    {
	if (isspace(source.end[last])) break;
	if (++last == 0)
	{
	    trailing = 0;
	    break;
	}
    }

    /* Get address and length of the token */
    parsed.data = source.end + first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    STK(1) = (cell_t) parsed.data;
    TOP = parsed.length;
}
#endif

void mu_token()
{
    ssize_t last;
    int trailing;

    /* Skip leading whitespace */
    for (;;)
    {
	if (first == 0)
	    break;

	if (!isspace(source.end[first]))
	    break;

	first++;
    }

    /*
     * Now scan for trailing delimiter and consume it,
     * unless we run out of input text first.
     */
    last = first;
    trailing = 1;
    for (;;)
    {
	if (last == 0)
	{
	    trailing = 0;
	    break;
	}
	if (isspace(source.end[last]))
	    break;

	last++;
    }

    /* Get address and length of the token */
    parsed.data = source.end + first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    STK(-1) = (cell_t) parsed.data;
    STK(-2) = parsed.length;
    DROP(-2);
}

#if 0
/* Parse token using arbitrary delimiter; do not skip leading delimiters */
void mu_parse_old()
{
    ssize_t last;
    int trailing;
    int c;

    c = TOP;

    /* regardless of the outcome, we return (start, length) */
    DROP(-1);

    if (first == 0)
    {
	STK(1) = TOP = 0;
	return;
    }
	
    last = first;

    /*
     * Now scan for trailing delimiter and consume it,
     * unless we run out of input text first.
     */
    trailing = 1;
    for (;;)
    {
	if (c == source.end[last]) break;
	if (++last == 0)
	{
	    trailing = 0;
	    break;
	}
    }

    /* Get address and length of the token */
    parsed.data = source.end + first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    STK(1) = (cell_t) parsed.data;
    TOP = parsed.length;
}
#endif

void mu_parse()
{
    ssize_t last;
    int trailing;
    int c;

    c = TOP;

    /* The first character of unseen input is the first character of token */
    last = first;

    /*
     * Scan for trailing delimiter and consume it,
     * unless we run out of input text first.
     */
    trailing = 1;
    for (;;)
    {
	if (last == 0)
	{
	    trailing = 0;
	    break;
	}
	if (c == source.end[last])
	    break;

	++last;
    }

    /* Get address and length of the token */
    parsed.data = source.end + first;
    parsed.length = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    TOP = (cell_t) parsed.data;
    STK(-1) = parsed.length;
    DROP(-1);
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

void mu_huh()
{
    if (POP) return;
    mu_complain();
}

void (*mu_number)() = mu_complain;
void (*mu_number_comma)() = mu_complain;

void mu_push_tick_number()
{
    PUSH(&mu_number);
}

void mu_push_tick_number_comma()
{
    PUSH(&mu_number_comma);
}

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
    execute((cell_t) mu_number);
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
	mu_compile_call();
	return;
    }
    execute((cell_t) mu_number_comma);
}

void mu_nope() {}		/* very useful NO-OP */
void mu_zzz() {}		/* a convenient GDB breakpoint */

/*
 * Remember that the second part of a struct imode is a pointer to code to
 * print a mode-specific prompt. The muforth kernel lacks decent I/O
 * facilities. Until these are defined in startup.mu4, the prompts are
 * noops.
 */
static struct imode forth_interpreter  = { mu__lbracket, mu_nope };
static struct imode forth_compiler     = { mu__rbracket, mu_nope };

static struct imode *state = &forth_interpreter;

void mu_push_state()
{
    PUSH(&state);
}

void mu_lbracket()
{
    state = &forth_interpreter;
}

void mu_minus_rbracket()
{
    state = &forth_compiler;
}

void mu_push_parsed()
{
    STK(-1) = (cell_t) parsed.data;
    STK(-2) = parsed.length;
    DROP(-2);
}

static void mu_qstack()
{
    if (sp > S0)
    {
	PUSH(ate_the_stack);
	mu_throw();
    }
}

void mu_interpret()
{
    source.end = (char *) STK(1) + TOP;	/* the _end_ of the text */
    source.start = -TOP;		/* offset to the start of text */
    DROP(2);

    first = source.start;

    for (;;)
    {
	mu_token();
	if (TOP == 0) break;
	execute((cell_t) state->eat);	/* consume(); */
	mu_qstack();
    }
    DROP(2);
}

void mu_evaluate()
{
    struct text saved_source;
    ssize_t saved_first;

    saved_source = source;
    saved_first = first;

    PUSH(mu_interpret);
    mu_catch();
    source = saved_source;
    first = saved_first;
    mu_throw();
}

void mu_start_up()
{
    PUSH("warm");		/* push the token "warm" */
    PUSH(4);
    mu__lbracket();		/* ... and execute it! */
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

