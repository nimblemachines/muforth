/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* the very basic words */

#include "muforth.h"
#include "version.h"

void add()
{
    int x = POP;
    TOP += x;
}

void and()
{
    int x = POP;
    TOP &= x;
}

void or()
{
    int x = POP;
    TOP |= x;
}

void xor()
{
    int x = POP;
    TOP ^= x;
}

void negate()
{
    TOP = -TOP;
}

void invert()
{
    TOP = ~TOP;
}

void two_star() { TOP = TOP << 1; }
void two_slash() { TOP = TOP >> 1; }
void two_slash_unsigned() { TOP = ((unsigned int) TOP) >> 1; }

void shift_left()
{
    int sh = POP;
    TOP <<= sh;
}

void shift_right()
{
    int sh = POP;
    TOP >>= sh;
}

void shift_right_unsigned()
{
    int sh = POP;
    (unsigned) TOP >>= sh;
}

void fetch()
{
    TOP = *(int *) TOP;
}

void cfetch()
{
    TOP = *(u_int8_t *) TOP;
}

void store()
{
    int *p = (int *) TOP;

    *p = STK(1);
    DROP(2);
}

void cstore()
{
    u_int8_t *p = (u_int8_t *) TOP;

    *p = (u_int8_t) STK(1);
    DROP(2);
}

void plus_store()
{
    int *p = (int *) TOP;

    *p += STK(1);
    DROP(2);
}

void const_one() { PUSH(1); }

void rot()
{
    int t = TOP;
    TOP = STK(2);
    STK(2) = STK(1);
    STK(1) = t;
}

void minus_rot()
{
    int t = TOP;
    TOP = STK(1);
    STK(1) = STK(2);
    STK(2) = t;
}

void dupe()
{
    int t = TOP;
    PUSH(t);
}

void nip()
{
    int t = POP;
    TOP = t;
}

void swap()
{
    int t = TOP;
    TOP = STK(1);
    STK(1) = t;
}

void over()
{
    int s = STK(1);
    PUSH(s);
}

void tuck()  /* a b - b a b */
{
    int t = TOP;
    TOP = STK(1);
    STK(1) = t;
    PUSH(t);
}

void string_equal()
{
    struct string s1, s2;

    s2.len = POP;
    s2.data = (char *) POP;
    s1.len = POP;
    s1.data = (char *) POP;

    if (s1.len == s2.len && memcmp(s1.data, s2.data, s1.len) == 0)
	PUSH(-1);
    else
	PUSH(0);
}

void uless()
{
    STK(1) = (STK(1) < (uint) TOP) ? -1 : 0;
    DROP(1);
}

void less()
{
    STK(1) = (STK(1) < TOP) ? -1 : 0;
    DROP(1);
}

void zless()
{
    TOP = (TOP < 0) ? -1 : 0;
}

void zequal()
{
    TOP = (TOP == 0) ? -1 : 0;
}

void sp_fetch()
{
    PUSH(sp);
}

void sp_store()
{
    sp = TOP;
    DROP(1);
}

void cmove()
{
    void *src = (void *) STK(2);
    void *dest = (void *) STK(1);
    size_t count = TOP;

    memcpy(dest, src, count);
    DROP(3);
}

void string_length()
{
    TOP = strlen((char *) TOP);
}

void push_version()
{
    PUSH(version);
    PUSH(strlen(version));
}

void push_build_time()
{
    PUSH(build_time);
}

void push_argc()
{
    PUSH(cmd_line_argc);
}

void push_argv()
{
    PUSH(cmd_line_argv);
}

