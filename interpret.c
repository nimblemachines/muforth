/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* interpreter and compiler */

#include "muforth.h"

#include <ctype.h>

struct imode			/* interpreter mode */
{
    void (*eat)();		/* consume one token */
    void (*prompt)();
};

static struct string source;
static size_t to_in;

struct string parsed;		/* for errors */

/* parse blank-delimited tokens */
void token()
{
    char *end;
    int first;
    int last;
    int eat_trailing;

    first = to_in - source.len;
    if (first == 0)
    {
	STK(-1) = STK(-2) = 0;
	DROP(-2);
	return;
    }
	
    end = source.data + source.len;

    /* skip leading whitespace */
    for (;;)
    {
	if (!isspace(end[first])) break;
	if (++first == 0)
	{
	    to_in = first + source.len;
	    STK(-1) = STK(-2) = 0;
	    DROP(-2);
	    return;
	}
    }

    /* now scan for trailing delimiter */
    last = first;
    eat_trailing = 1;
    for (;;)
    {
	if (isspace(end[last])) break;
	if (++last == 0)
	{
	    eat_trailing = 0;
	    break;
	}
    }

    /* account for characters processed, return token */
    to_in = last + eat_trailing + source.len;
    parsed.data = (end + first);
    parsed.len = last - first;

    STK(-1) = (int) parsed.data;
    STK(-2) = parsed.len;
    DROP(-2);
}

/* parse non-blank-delimited tokens */
void parse()
{
    char *end;
    int first, last;
    int eat_trailing;
    int c;

    c = TOP;
    first = last = to_in - source.len;
    if (first == 0)
    {
	TOP = STK(-1) = 0;
	DROP(-1);
	return;
    }
	
    end = source.data + source.len;

    /* scan for trailing delimiter */
    eat_trailing = 1;
    for (;;)
    {
	if (c == end[last]) break;
	if (++last == 0)
	{
	    eat_trailing = 0;
	    break;
	}
    }

    /* account for characters processed, return token */
    to_in = last + eat_trailing + source.len;
    parsed.data = (end + first);
    parsed.len = last - first;

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
void zzz() {}		/* a convenient breakpoint */

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

/* XXX: macro or inline? */
static void consume()
{
    (*state->eat)();
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
    source.data = (char *) STK(1);
    source.len = TOP;
    DROP(2);
    to_in = 0;

    for (;;)
    {
	token();
	if (TOP == 0) break;
	consume();
	qstack();
    }
    DROP(2);
}

void evaluate()
{
    struct string saved_source = source;
    size_t saved_to_in = to_in;

    PUSH(interpret);
    catch();
    source = saved_source;
    to_in = saved_to_in;
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

