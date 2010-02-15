/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* "Multi-precision" integer arithmetic. */

/*
 * This file only exists because both C and GCC are incredibly lame. The
 * instructions exist on most processors for me to do what I want - namely,
 * multiply two 32 bit ints and get a 64 bit result, and also divide a 64
 * bit int by a 32 bit int, and get a 32 bit quotient and remainder. But I
 * can't seem to tell the C compiler that this is what I want. Among other
 * things, C stupidly lacks any notion of the carry bit, so doing
 * multi-precision arithmetic in the natural word size of the machine is
 * impossible.
 *
 * GCC claims to be able to do this kind of "double-length" integer math,
 * but when I've tried using "long long" the generated code was a joke, if
 * not completely wrong. It was making me crazy.
 *
 * Then I remembered reading about Python's implementation, which uses 16
 * bit "digits" on a 32-bit machine, and thought "aha!".
 *
 * We have to cheat, and totally waste the machine. But at least we get
 * what we want. We're going to define a new type - bigit - which is a "big
 * digit". We will carefully and intentionally choose its size to be 16
 * bits - exactly half what the "natural" int is on this machine, and since
 * the compiler -will- give us 32x32 multiply that yields a 32 bit result,
 * if we leave the high bits of each multiplicand zero, then a
 * "double-precision" (that is, twice 16 bits!) result will fit.
 *
 * From these bigits we will synthesise 64 bit math. The interesting thing
 * is that we're actually going to develop -arbitrary- precision
 * arithmetic, but then use it "only" for numbers with 2 or 4 bigits (32
 * bit and 64 bit quantities).
 *
 * Ready?
 */

#include "muforth.h"

typedef u_int16_t bigit;
typedef u_int32_t two_bigit;    /* twice as wide */

int mp_add(int carry, bigit *p, bigit *q, int size)
{
    two_bigit accum;

    while (size-- > 0)
    {
        accum = *p + *q++ + carry;
        carry = (accum >> 16) ? 1 : 0;
        *p++ = accum;
    }
    return carry;
}

int mp_negate(int carry, bigit *p, int size)
{
    two_bigit accum;

    while (size-- > 0)
    {
        bigit notp = ~(*p);
        accum = notp + carry;
        carry = (accum >> 16) ? 1 : 0;
        *p++ = accum;
    }
    return carry;
}

void mp_mult(bigit *p, bigit *q, bigit *result, int size)
{
    two_bigit accum;
    two_bigit *z;       /* for more quickly zeroing result */
    int i, j;

    for (i = 0, z = (two_bigit *)result; i < size; i++)
        *z++ = 0;

    for (i = 0; i < size; i++)
        for (j = 0; j < size; j++)
        {
            accum = p[i] * q[j];
            result[i+j]   += accum;         /* lo */
            result[i+j+1] += accum >> 16;   /* hi */
        }
}

/* Now for some Forth words. */

/* d+ ( alo ahi blo bhi - sumlo sumhi) */
void mu_dplus()
{
    int carry;
    carry = mp_add(0,     (bigit *)&ST3, (bigit *)&ST1, 2);
            mp_add(carry, (bigit *)&ST2, (bigit *)&TOP, 2);
    DROP(2);
}

/* dnegate ( lo hi - -lo ~hi+carry) */
void mu_dnegate()
{
    int carry;
    carry = mp_negate(1, (bigit *)&ST1, 2);
            mp_negate(carry, (bigit *)&TOP, 2);
}

/* um* ( u1 u2 - uprodlo uprodhi) */
void mu_um_star()
{
    cell result[2];

    mp_mult((bigit *)&ST1, (bigit *)&TOP, (bigit *)&result, 2);
    ST1 = result[0];
    TOP = result[1];
}


