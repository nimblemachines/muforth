/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* basic tools to compile code and data */

#include "muforth.h"

void push_h()			/* "here" code space pointer */
{
    PUSH(&pcd);
}

void push_r()			/* "ram" space pointer */
{
    PUSH(&pdt);
}

void push_s0()			/* address of stack bottom */
{
    PUSH(S0);
}

void push_sp()			/* address of sp variable */
{
    PUSH(&sp);
}

/* copy string; return a counted string (addr of prefix count byte);
   ( a u c" - c") */
void scrabble()
{
    struct string s;
    char *dest;

    dest = (char *) TOP;
    s.data = (char *) STK(2);
    *dest = s.len = STK(1);	/* prefix count byte */
    memcpy(dest + 1, s.data, s.len);
    *(dest + s.len + 1) = 0;	/* zero terminate */
    STK(2) = (int) dest;
    DROP(2);
}

void colon()
{
    make_new_name();
    minus_rbracket();
}

void minus_semicolon()
{
    lbracket();
}

void semicolon()
{
    compile_return();
    minus_semicolon();
}

/*
compiler definitions
: -;    \ [  ;             ( for words that don't end with ^)
: ^   lit unnest compile,  ;   ( right now this doesn't do anything fancy)
: ;     \ ^  \ -;  ;
forth definitions
*/


