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

void mu_add()
{
    int x = POP;
    TOP += x;
}

void mu_and()
{
    int x = POP;
    TOP &= x;
}

void mu_or()
{
    int x = POP;
    TOP |= x;
}

void mu_xor()
{
    int x = POP;
    TOP ^= x;
}

void mu_negate()
{
    TOP = -TOP;
}

void mu_invert()
{
    TOP = ~TOP;
}

void mu_two_star() { TOP = TOP << 1; }
void mu_two_slash() { TOP = TOP >> 1; }
void mu_two_slash_unsigned() { TOP = ((unsigned int) TOP) >> 1; }

void mu_shift_left()
{
    int sh = POP;
    TOP <<= sh;
}

void mu_shift_right()
{
    int sh = POP;
    TOP >>= sh;
}

void mu_shift_right_unsigned()
{
    int sh = POP;
    (unsigned) TOP >>= sh;
}

void mu_fetch()
{
    TOP = *(int *) TOP;
}

void mu_cfetch()
{
    TOP = *(u_int8_t *) TOP;
}

void mu_store()
{
    int *p = (int *) TOP;

    *p = STK(1);
    DROP(2);
}

void mu_cstore()
{
    u_int8_t *p = (u_int8_t *) TOP;

    *p = (u_int8_t) STK(1);
    DROP(2);
}

void mu_plus_store()
{
    int *p = (int *) TOP;

    *p += STK(1);
    DROP(2);
}

void mu_rot()
{
    int t = TOP;
    TOP = STK(2);
    STK(2) = STK(1);
    STK(1) = t;
}

void mu_minus_rot()
{
    int t = TOP;
    TOP = STK(1);
    STK(1) = STK(2);
    STK(2) = t;
}

void mu_dupe()
{
    int t = TOP;
    PUSH(t);
}

void mu_nip()
{
    int t = POP;
    TOP = t;
}

void mu_swap()
{
    int t = TOP;
    TOP = STK(1);
    STK(1) = t;
}

void mu_over()
{
    int s = STK(1);
    PUSH(s);
}

void mu_tuck()  /* a b - b a b */
{
    int t = TOP;
    TOP = STK(1);
    STK(1) = t;
    PUSH(t);
}

void mu_string_equal()
{
    struct string s1, s2;

    s1.data = (char *) STK(3);
    s1.len  =          STK(2);
    s2.data = (char *) STK(1);
    s2.len  =          TOP;
    DROP(3);

    if (s1.len == s2.len && memcmp(s1.data, s2.data, s1.len) == 0)
	TOP = -1;
    else
	TOP = 0;
}

void mu_uless()
{
    STK(1) = (STK(1) < (uint) TOP) ? -1 : 0;
    DROP(1);
}

void mu_less()
{
    STK(1) = (STK(1) < TOP) ? -1 : 0;
    DROP(1);
}

void mu_zless()
{
    TOP = (TOP < 0) ? -1 : 0;
}

void mu_zequal()
{
    TOP = (TOP == 0) ? -1 : 0;
}

void mu_sp_fetch()
{
    PUSH(sp);
}

void mu_sp_store()
{
    sp = (int *) TOP;
}

void mu_cmove()
{
    void *src = (void *) STK(2);
    void *dest = (void *) STK(1);
    size_t count = TOP;

    memcpy(dest, src, count);
    DROP(3);
}

void mu_string_length()
{
    TOP = strlen((char *) TOP);
}

void mu_push_version()
{
    PUSH(version);
    PUSH(strlen(version));
}

void mu_push_build_time()
{
    PUSH(build_time);
}

void mu_push_argc()
{
    PUSH(cmd_line_argc);
}

void mu_push_argv()
{
    PUSH(cmd_line_argv);
}

