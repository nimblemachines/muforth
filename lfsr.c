/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2014 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* investigations into LFSRs - linear feedback shift registers - for
 * pseudo-random number generation.
 */

#include "muforth.h"

/* Use ucells to hold bits. They are 32-bits and unsigned. */

/* Galois style. Taps at 32, 22, 2, 1 */

void mu_lfsr_gal_length()
{
    ucell bits = 0xffffffff;
    ucell count = 0;
    ucell mask = (1 << 31) | (1 << 21) | 3;

    do
    {
        ++count;
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    } while (bits != 0xffffffff);
    PUSH(count);
}

void mu_lfsr_gal()
{
    val count = TOP;
    ucell bits = ST1;
    ucell mask = (1 << 31) | (1 << 21) | 3;

    while (count-- > 0)
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    DROP(1);
    TOP = bits;
}

void mu_lfsr_fib_length()
{
    ucell bits = 0xffffffff;
    ucell count = 0;
    ucell bit;

    do
    {
        ++count;
        bit = ((bits >> 31) ^ (bits >> 21) ^ (bits >> 1) ^ bits) & 1;
        bits = (bits << 1) + bit;
    } while (bits != 0xffffffff);
    PUSH(count);
}

void mu_lfsr_fib()
{
    val count = TOP;
    ucell bits = ST1;
    ucell bit;

    while (count-- > 0)
    {
        bit = ((bits >> 31) ^ (bits >> 21) ^ (bits >> 1) ^ bits) & 1;
        bits = (bits << 1) + bit;
    }
    DROP(1);
    TOP = bits;
}

/* Galois #2: Taps: 32, 30, 26, 25. The advantage that they are all in the
 * high byte!
 */

void mu_lfsr_gal_two_length()
{
    ucell bits = 0xffffffff;
    ucell count = 0;
    ucell mask = (1 << 31) | (1 << 29) | (1 << 25) | (1 << 24);

    do
    {
        ++count;
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    } while (bits != 0xffffffff);
    PUSH(count);
}

void mu_lfsr_gal_two()
{
    val count = TOP;
    ucell bits = ST1;
    ucell mask = (1 << 31) | (1 << 29) | (1 << 25) | (1 << 24);

    while (count-- > 0)
        bits = (bits >> 1) ^ (-(bits & 1) & mask);
    DROP(1);
    TOP = bits;
}
