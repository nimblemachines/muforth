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

/* the char *string param here does _not_ need to be zero-terminated!! */
static char *compile_counted_string(char *string, size_t length, void *dest)
{
    struct counted_string *cs;
    struct string s;

    cs = (struct counted_string *)dest;
    s.data = string;
    s.len = length;
    cs->len = s.len;		/* prefix count cell */
    memcpy(&cs->data[0], s.data, s.len);
    cs->data[s.len] = 0;	/* zero terminate */
    return &cs->data[0];
}

/* copy string; return a counted string (addr of first character;
 * prefix count cell _precedes_ first character of string).
 * This does _not_ allot space in the dictionary!
 */
void mu_scrabble()  /* ( a u z" - z") */
{
    STK(2) = (int)compile_counted_string((char *)STK(2), STK(1), (void *)TOP);
    DROP(2);
}

/* this is _only_ called from C; zstring _must_ be zero-terminated!!
 * this routine _does_ allot space in the dictionary! */
char *to_counted_string(char *zstring)
{
    size_t length;
    char *counted_string;

    length = strlen(zstring);
    counted_string = compile_counted_string(zstring, length, pdt);
    pdt = (u_int8_t *)ALIGNED(counted_string + length + 1);  /* count null */
    return counted_string;
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


