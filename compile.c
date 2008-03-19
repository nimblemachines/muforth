/*
 * This file is part of muFORTH: http://pages.nimblemachines.com/muforth
 *
 * Copyright (c) 1997-2008 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* basic tools to compile code and data */

#include "muforth.h"

void mu_push_h()        /* "here" code space pointer */
{
    PUSH(&pcd);
}

void mu_push_r()        /* "ram" space pointer */
{
    PUSH(&pdt);
}

/*
 * Compile (that is, *copy*) a cell into the current code space.
 */
void mu_code_comma()     { *pcd++ = POP; }

void mu_push_s0()       /* address of stack bottom */
{
    PUSH(S0);
}

/* the char *string param here does _not_ need to be zero-terminated!! */
static char *compile_counted_string(char *string, size_t length)
{
    struct counted_string *cs;
    struct string s;

    cs = (struct counted_string *)pdt;
    s.data = string;
    s.length = length;
    cs->length = s.length;  /* prefix count cell */
    bcopy(s.data, &cs->data[0], s.length);
    cs->data[s.length] = 0; /* zero terminate */
    return &cs->data[0];
}

/*
 * copy string; return a counted string (addr of first character;
 * prefix count cell _precedes_ first character of string).
 * This does _not_ allot space in the dictionary!
 */
void mu_scrabble()  /* ( a u - z") */
{
    TOP = (cell)compile_counted_string((char *)ST1, TOP);
    NIP(1);
}

/*
 * this is _only_ called from C; zstring _must_ be zero-terminated!!
 * this routine _does_ allot space in the dictionary!
 */
char *to_counted_string(char *zstring)
{
    size_t length;
    char *counted_string;

    length = strlen(zstring);
    counted_string = compile_counted_string(zstring, length);
    pdt = (uint8 *)ALIGNED(counted_string + length + 1);  /* count null */
    return counted_string;
}

void mu_colon()
{
    mu_new();
    mu_set_colon_code();
    mu_minus_rbracket();
}

/*
  compiler definitions
  : ^   lit unnest compile,  ;   ( right now this doesn't do anything fancy)
  : ;     \ ^  \ [  ;
  forth definitions
*/


