/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
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


#ifdef ALTERNATE_MP_MATH
/*
 * Let's try using full cells - rather than half cells (bigits) for
 * addition and negation.
 */
void mu_dnegate2()
{
    ST1 = -ST1;     /* negate low half */
    TOP = ~TOP;     /* one's complement high half */
    if (ST1 == 0) TOP++;    /* propagate carry if low was zero */
}

void mu_dnegate3()
{
    ST1 = -ST1;     /* negate low half */
    TOP = (ST1 == 0) ? -TOP : ~TOP;
}

void mu_dplus2()
{
    dcell *a = &ST2;    /* point to high half */
    dcell *b = &TOP;    /* ditto */

    a->lo += b->lo;      /* sum low halves */
    a->hi += b->hi;      /* ... and high halves */
    if ((unsigned)a->lo < b->lo)    /* sum u< b, there was a carry! */
        a->hi++;

    DROP(2);
}

void mu_dplus3()
{
    dcell *a = &ST2;    /* point to high half */
    dcell *b = &TOP;    /* ditto */
    int carry;

    a->lo += b->lo;      /* sum low halves */
    carry = ((unsigned)a->lo < b->lo) ? 1 : 0;    /* sum u< b ==> carry! */
    a->hi += b->hi + carry;  /* sum high halves and carry */

    DROP(2);
}
#endif /* ALTERNATE_MP_MATH */

/*
 * This is now an archive of routines moved _out_ of kernel.c. I don't want
 * to get rid of the completely - even though they are not used in muforth!
 * - so I put them here.
 */

/*
 * Single-length math routines.
 *
 * Sometimes I really hate C and gcc. This is one of those times. It is
 * trivially easy to write the basic Forth word fm/mod in assembler. The
 * machine gives you the pieces you need: given two 32bit operands, it
 * multiplies and gives a 64bit result; then you divide that by another
 * 32bit operand. There is no opportunity for over- or underflow, and it's
 * about 6 instructions - including stack moves - in x86 assembler.
 *
 * It's impossible to do this in gcc. Grrr.
 *
 * Also, since integer divide by definition gives you both quotient and
 * remainder, why does C make you calculate them separately? It's stupid.
 *
 * So, I've given up on double-length math for muforth. It's a beautiful
 * and elegant part of Forth, but since I intend muforth mostly for
 * cross-compiling (to 32bit architectures at the moment, though that could
 * change!), single-length is plenty. So don't try using star-slash with
 * large operands. ;-)
 */

#ifdef GCC_IS_COMPLETELY_FUCKED
/*
 * Some tests that didn't work out. I think the fault was intially mine: I
 * had cast using "(unsigned)" rather than "(uint32_t)". That may have made
 * a difference. It certainly caused my initial 64-bit port to work in
 * every way except when doing unsigned math, and that was the culprit
 * there.
 *
 * But I'm still willing to believe that GCC is completely fucked. ;-)
 */
void mu_umstar()
{
    uint64_t prod = (uint32_t)ST1 * TOP;
    cell *p = &prod;
    ST1 = p[0];     /* low half of product */
    TOP = p[1];     /* high half */
}

void mu_mstar()
{
    int64_t prod = ST1 * TOP;
    cell *p = &prod;
    ST1 = p[0];     /* low half of product */
    TOP = p[1];     /* high half */
}

void mu_fm_slash_mod()  /* dn1 n2 -- m q */
{
    cell num[2];    /* numerator (dividend) */
    cell mod;
    cell quot;
    int64_t dividend;

    /* Set up 64-bit dividend */
    num[0] = ST2;   /* low half */
    num[1] = ST1;   /* high half */
    quot = (int64_t)num[0] / TOP;
    mod  = (int64_t)num[0] % TOP;

    /* Set up 64-bit dividend */
    dividend = ST2 /* low */ + ((int64_t)ST1 << 32) /* high */ ;
    quot = dividend / TOP;
    mod  = dividend % TOP;
#ifdef DIVIDE_IS_SYMMETRIC
    /*
     * We now have the results of a stupid symmetric division, which we
     * must convert to floored. We only do this if the modulus was non-zero
     * and if the dividend and divisor had opposite signs.
     */
    if (mod != 0 && (ST1 ^ TOP) < 0)
    {
        quot -= 1;
        mod  += TOP;
    }
#endif

    ST1 = mod;
    TOP = quot;
}
#endif /* GCC_IS_COMPLETELY_FUCKED */

