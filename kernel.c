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

#define MIN(a,b)	(((a) < (b)) ? (a) : (b))

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

/*
 * Like C and unlike Forth, mu_string_compare returns an integer representing
 * an ordering (in general the difference between the ASCII codes of the first
 * two non-matching characters):
 *
 *  <0 means the first string is "less";
 *   0 means the two strings are equal;
 *  >0 means the first string is "greater".
 *
 * If two strings are the same length, then:
 *   If every character is equal, 0 is returned;
 *   Else, the ordering (difference) of their first non-equal characters
 *     is returned.
 *
 * If the two strings are of different lengths, then:
 *   If they share the same prefix, the shorter string is "less"; the shorter
 *     string is treated as if it had a last character of 0.
 *   Else, the ordering (difference) of their first non-equal characters
 *     is returned.
 *   Note that in this second case, 0 is never returned.
 */
void mu_string_compare()
{
    STK(3) = string_compare((char *)STK(3), STK(2), (char *)STK(1), TOP);
    DROP(3);
}

int string_compare(const char *string1, size_t length1,
		   const char *string2, size_t length2)
{
    int ordering;

    /* Careful: if lengths differ the strings can't compare as equal! */
    if (length1 == length2)
	ordering = strncmp(string1, string2, length1);
    else
    {
	int cmp;

	/* Compare as many characters as we can */
	cmp = strncmp(string1, string2, MIN(length1, length2));

	/*
	 * If all equal, then their lengths determine the outcome (the
	 * shorter string is "less"). Otherwise, use the result of the
	 * strncmp (which tells us how the first characters that differed
	 * differed).
	 */
	if (cmp == 0)
	{
	    if (length1 < length2)
		ordering = -string2[length1];
	    else
		ordering =  string1[length2];
	}
	else
	    ordering = cmp;
    }
    return ordering;
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
