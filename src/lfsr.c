/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/*
 * Investigations into LFSRs - linear feedback shift registers - for
 * pseudo-random number generation.
 */

#include "muforth.h"

/*
 * NOTE: The word length of muforth cells has changed from 32 bits to 64
 * bits since this code was written!
 *
 * I have re-written the 32-bit shift registers as uint32_t rather than
 * ucell, and added a 64-bit shift register, just for fun.
 */

/* Galois style. Taps at 32, 22, 2, 1 */

void mu_lfsr_gal_length()
{
    uint32_t bits = 0xffffffff;
    ucell count = 0;
    uint32_t mask = (1U << 31) | (1 << 21) | 3;

    do
    {
        ++count;
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    } while (bits != 0xffffffff);
    PUSH(count);
}

void mu_lfsr_gal()  /* lfsr-gal ( oldbits #bits-to-generate -- newbits) */
{
    ucell count = TOP;
    uint32_t bits = ST1;
    uint32_t mask = (1U << 31) | (1 << 21) | 3;

    while (count-- > 0)
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    DROP(1);
    TOP = bits;
}

void mu_lfsr_fib_length()
{
    uint32_t bits = 0xffffffff;
    ucell count = 0;
    uint32_t bit;

    do
    {
        ++count;
        bit = ((bits >> 31) ^ (bits >> 21) ^ (bits >> 1) ^ bits) & 1;
        bits = (bits << 1) + bit;
    } while (bits != 0xffffffff);
    PUSH(count);
}

void mu_lfsr_fib()  /* lfsr-fib ( oldbits #bits-to-generate -- newbits) */
{
    ucell count = TOP;
    uint32_t bits = ST1;
    uint32_t bit;

    while (count-- > 0)
    {
        bit = ((bits >> 31) ^ (bits >> 21) ^ (bits >> 1) ^ bits) & 1;
        bits = (bits << 1) + bit;
    }
    DROP(1);
    TOP = bits;
}

/*
 * Galois #2: Taps: 32, 30, 26, 25. The advantage that they are all in the
 * high byte!
 */

void mu_lfsr_gal_two_length()
{
    uint32_t bits = 0xffffffff;
    ucell count = 0;
    uint32_t mask = (1U << 31) | (1 << 29) | (1 << 25) | (1 << 24);

    do
    {
        ++count;
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    } while (bits != 0xffffffff);
    PUSH(count);
}

void mu_lfsr_gal_two()  /* lfsr-gal-two ( oldbits #bits-to-generate -- newbits) */
{
    ucell count = TOP;
    uint32_t bits = ST1;
    uint32_t mask = (1U << 31) | (1 << 29) | (1 << 25) | (1 << 24);

    while (count-- > 0)
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    DROP(1);
    TOP = bits;
}

/*
 * 64-bit Galois shift register. Taps at 64, 63, 61, and 60. Unfortunately
 * we cannot simply run it to see its period! It would take too long. ;-)
 */
void mu_lfsr_gal64()    /* lfsr-gal64 ( oldbits #bits-to-generate -- newbits) */
{
    ucell count = TOP;
    ucell bits = ST1;
    ucell mask = (1UL << 63) | (1UL << 62) | (1UL << 60) | (1UL << 59);

    while (count-- > 0)
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    DROP(1);
    TOP = bits;
}
