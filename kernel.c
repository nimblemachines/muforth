/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* the very basic words */

#include "muforth.h"

void mu_add()
{
    cell_t x = POP;
    TOP += x;
}

void mu_and()
{
    cell_t x = POP;
    TOP &= x;
}

void mu_or()
{
    cell_t x = POP;
    TOP |= x;
}

void mu_xor()
{
    cell_t x = POP;
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
void mu_two_slash_unsigned() { TOP = ((unsigned) TOP) >> 1; }

void mu_shift_left()
{
    cell_t sh = POP;
    TOP <<= sh;
}

void mu_shift_right()
{
    cell_t sh = POP;
    TOP >>= sh;
}

void mu_shift_right_unsigned()
{
    cell_t sh = POP;
    (unsigned) TOP >>= sh;
}

void mu_fetch()
{
    TOP = *(cell_t *) TOP;
}

void mu_cfetch()
{
    TOP = *(uint8_t *) TOP;
}

void mu_store()
{
    cell_t *p = (cell_t *) TOP;

    *p = STK(1);
    DROP(2);
}

void mu_cstore()
{
    uint8_t *p = (uint8_t *) TOP;

    *p = (uint8_t) STK(1);
    DROP(2);
}

void mu_plus_store()
{
    cell_t *p = (cell_t *) TOP;

    *p += STK(1);
    DROP(2);
}

void mu_rot()
{
    cell_t t = TOP;
    TOP = STK(2);
    STK(2) = STK(1);
    STK(1) = t;
}

void mu_minus_rot()
{
    cell_t t = TOP;
    TOP = STK(1);
    STK(1) = STK(2);
    STK(2) = t;
}

void mu_dupe()
{
    cell_t t = TOP;
    PUSH(t);
}

void mu_nip()
{
    cell_t t = POP;
    TOP = t;
}

void mu_swap()
{
    cell_t t = TOP;
    TOP = STK(1);
    STK(1) = t;
}

void mu_over()
{
    cell_t s = STK(1);
    PUSH(s);
}

void mu_tuck()  /* a b - b a b */
{
    cell_t t = TOP;
    TOP = STK(1);
    STK(1) = t;
    PUSH(t);
}

void mu_string_equal()
{
    struct string s1, s2;

    s1.data   = (char *) STK(3);
    s1.length =          STK(2);
    s2.data   = (char *) STK(1);
    s2.length =          TOP;
    DROP(3);

    if (s1.length == s2.length && memcmp(s1.data, s2.data, s1.length) == 0)
	TOP = -1;
    else
	TOP = 0;
}

void mu_uless()
{
    STK(1) = (STK(1) < (unsigned) TOP) ? -1 : 0;
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
    sp = (cell_t *) TOP;
}

void mu_cmove()
{
    void *src = (void *) STK(2);
    void *dest = (void *) STK(1);
    size_t count = TOP;

    memcpy(dest, src, count);
    DROP(3);
}

/* just for fun. tests of how lame gcc still is. */
void mu_dplus_gcc()
{
    d_cell_t *dsp = (d_cell_t *)sp;
    d_arith_t sum;

    sum = MAKE_DBL(dsp->hi, dsp->lo);
    sum += MAKE_DBL(dsp[1].hi, dsp[1].lo);
    dsp[1].hi = HI(sum);
    dsp[1].lo = LO(sum);
    sp = (cell_t *)&dsp[1];	/* drop one double */
}

void mu_dnegate_gcc()
{
    d_cell_t *dsp = (d_cell_t *)sp;
    d_arith_t neg;

    neg = -MAKE_DBL(dsp->hi, dsp->lo);
    dsp->hi = HI(neg);
    dsp->lo = LO(neg);
}


