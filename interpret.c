/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* Interpreter and compiler */

#include "muforth.h"

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

/* Parse whitespace-delimited tokens */
void token()
{
    ssize_t last;
    int trailing;

    if (first == 0)
    {
	STK(-1) = STK(-2) = 0;
	DROP(-2);
	return;
    }
	
    /* Skip leading whitespace */
    for (;;)
    {
	if (!isspace(source.end[first])) break;
	if (++first == 0)
	{
	    STK(-1) = STK(-2) = 0;
	    DROP(-2);
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
    parsed.len = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    STK(-1) = (int) parsed.data;
    STK(-2) = parsed.len;
    DROP(-2);
}

/* Parse token using arbitrary delimiter; do not skip leading delimiters */
void parse()
{
    ssize_t last;
    int trailing;
    int c;

    c = TOP;

    if (first == 0)
    {
	TOP = STK(-1) = 0;
	DROP(-1);
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
    parsed.len = last - first;

    /* Account for characters processed, return token */
    first = last + trailing;

    TOP = (int) parsed.data;
    STK(-1) = parsed.len;
    DROP(-1);
}

/*
: complain   error"  is not defined"  -;
: huh?   if complain then  ;   ( useful after -"find or -' )
defer not-defined  now complain is not-defined
*/

void complain()
{
    PUSH("isn't defined");
    throw();
}

void huh()
{
    if (POP) complain();
}

void (*number)() = complain;
void (*number_comma)() = complain;

void push_tick_number()
{
    PUSH(&number);
}

void push_tick_number_comma()
{
    PUSH(&number_comma);
}

/* The interpreter's "consume" function. */
void _lbracket()
{
    forth_chain();
    minus_quote_find();
    if (POP)
    {
	(*number)();
	return;
    }
    EXECUTE;
}

/* The compiler's "consume" function. */
void _rbracket()
{
    compiler_chain();
    minus_quote_find();
    if (POP)
    {
	forth_chain();
	minus_quote_find();
	if (POP)
	{
	    (*number_comma)();
	    return;
	}
	compile_call();
	return;
    }
    EXECUTE;
}

void nope() {}		/* very useful NO-OP */
void zzz() {}		/* a convenient GDB breakpoint */

static struct imode forth_interpreter  = { _lbracket, nope };
static struct imode forth_compiler     = { _rbracket, nope };

static struct imode *state = &forth_interpreter;

void push_state()
{
    PUSH(&state);
}

void lbracket()
{
    state = &forth_interpreter;
}

void minus_rbracket()
{
    state = &forth_compiler;
}


void push_parsed()
{
    STK(-1) = (int) parsed.data;
    STK(-2) = parsed.len;
    DROP(-2);
}

static void qstack()
{
    if (sp > S0)
    {
	PUSH("ate the stack");
	throw();
    }
}

void interpret()
{
    source.end = (char *) STK(1) + TOP;	/* the _end_ of the text */
    source.start = -TOP;		/* offset to the start of text */
    DROP(2);

    first = source.start;

    for (;;)
    {
	token();
	if (TOP == 0) break;
	(*state->eat)();	/* consume(); */
	qstack();
    }
    DROP(2);
}

void evaluate()
{
    struct text saved_source;
    ssize_t saved_first;

    saved_source = source;
    saved_first = first;

    PUSH(interpret);
    catch();
    source = saved_source;
    first = saved_first;
    throw();
}

void start_up()
{
    PUSH("warm");		/* push the token "warm" */
    PUSH(4);
    _lbracket();		/* ... and execute it! */
}

void bye()
{
    exit(0);
}

/*
: consume   state @  @execute  ;

( interpret one token)
: _[
   forth -"find if  number  ^  then  execute  ;
   ( : 1interpret  <-executable?> if <number> ^ then  execute ; )

( compile one token)
: _]   compiler -"find  if  forth -"find  if   number,  ^  then
    compile,  ^  then    execute   ;
    ( : 1compile  <-executable?> if  <-compilable?> if <number,> ^ then
    <compile,> then  execute ; )

-:  compiler -"find if  assembler -"find if  number, exit  then
        compile, exit  then  execute  ;

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

