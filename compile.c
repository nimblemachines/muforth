/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2004 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* basic tools to compile code and data */

#include "muforth.h"

void mu_push_h()		/* "here" code space pointer */
{
    PUSH(&pcd);
}

void mu_push_r()		/* "ram" space pointer */
{
    PUSH(&pdt);
}

void mu_push_s0()		/* address of stack bottom */
{
    PUSH(S0);
}

void mu_push_sp()		/* address of sp variable */
{
    PUSH(&sp);
}

/* copy string; return a counted string (addr of first character;
 * prefix count cell _precedes_ first character of string).
 */
void mu_scrabble()  /* ( a u z" - z") */
{
    struct compiled_string *cs;
    struct string s;

    cs = (struct compiled_string *)TOP;
    s.data = (char *) STK(2);
    s.len = STK(1);
    cs->len = s.len;		/* prefix count cell */
    memcpy(&cs->data[0], s.data, s.len);
    cs->data[s.len] = 0;	/* zero terminate */
    STK(2) = (int) &cs->data[0];
    DROP(2);
}

void mu_colon()
{
    mu_make_new_name();
    mu_minus_rbracket();
}

void mu_minus_semicolon()
{
    mu_lbracket();
}

void mu_semicolon()
{
    mu_compile_return();
    mu_minus_semicolon();
}

/*
compiler definitions
: -;    \ [  ;             ( for words that don't end with ^)
: ^   lit unnest compile,  ;   ( right now this doesn't do anything fancy)
: ;     \ ^  \ -;  ;
forth definitions
*/


