/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
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

#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

cell mu_pop_dstack() { cell t = TOP; DROP(1); return t; }

void mu_plus() { TOP += ST1; NIP(1); }
void mu_and() { TOP &= ST1; NIP(1); }
void mu_or()  { TOP |= ST1; NIP(1); }
void mu_xor() { TOP ^= ST1; NIP(1); }

void mu_negate() { TOP = -TOP; }
void mu_invert() { TOP = ~TOP; }

void mu_2star()                  { TOP <<= 1; }
void mu_2slash()                 { TOP >>= 1; }
void mu_u2slash()  { TOP = (unsigned)TOP >>  1; }

void mu_shift_left()             { TOP = ST1 << TOP; NIP(1); }
void mu_shift_right()            { TOP = ST1 >> TOP; NIP(1); }
void mu_ushift_right() { TOP = (unsigned)ST1 >> TOP; NIP(1); }

void mu_fetch()  { TOP =    *(cell *) TOP; }
void mu_cfetch() { TOP = *(uint8_t *) TOP; }

void mu_store()       { *(cell *)TOP = ST1; DROP(2); }
void mu_cstore()   { *(uint8_t *)TOP = ST1; DROP(2); }
void mu_plus_store() { *(cell *)TOP += ST1; DROP(2); }

void mu_dup()  { DUP; }
void mu_nip()  { NIP(1); }
void mu_drop() { DROP(1); }
void mu_2drop() { DROP(2); }
void mu_swap() { cell t = TOP; TOP = ST1; ST1 = t; }
void mu_over() { DUP; TOP = ST2; }          /* a b -> a b a */

void mu_rot()       { cell t = TOP; TOP = ST2; ST2 = ST1; ST1 = t; }
void mu_minus_rot() { cell t = TOP; TOP = ST1; ST1 = ST2; ST2 = t; }

void mu_uless() { TOP = (ST1 < (unsigned) TOP) ? -1 : 0; NIP(1); }
void mu_less()  { TOP = (ST1 < TOP)            ? -1 : 0; NIP(1); }

void mu_zero_less()  { TOP = (TOP < 0)  ? -1 : 0; }
void mu_zero_equal() { TOP = (TOP == 0) ? -1 : 0; }

void mu_depth() { cell d = S0 - SP; PUSH(d); }
void mu_sp_reset() { SP = S0; TOP = 0xdecafbad; }

void mu_branch_()    { BRANCH; }
void mu_equal_zero_branch_() { if (TOP == 0) BRANCH; else IP++; }
void mu_zero_branch_()   { mu_equal_zero_branch_(); DROP(1); }

/* r stack functions */
void mu_push()   { RPUSH(POP); }
void mu_pop()    { PUSH(RPOP); }
void mu_rfetch() { PUSH(RP[0]); }

/* for, ?for, next */
/* for is simply "push" */
/* ?for has to matched with "then" */
void mu_qfor_()
{
    if (TOP == 0)
    {
        BRANCH;
        DROP(1);
    }
    else
    {
        IP++;
        RPUSH(POP);
    }
}

void mu_next_()
{
    cell *prtop;
    prtop = (cell *)&RP[0];

    if (--*prtop == 0)          /* decrement top of R stack */
    {
        IP += 1;                /* skip branch */
        RP += 1;                /* pop counter */
    }
    else
    {
        BRANCH;                 /* take branch */
    }
}

/*
 * C, or at least gcc, is sooooo fucking retarded! I cannot define "cell/"
 * the way I want, because gcc (on x86 at least) compiles /= by a power of
 * two of a _signed_ integer as an _un_signed_ shift! What gives!
 *
 * So I have to go to extra effort and circumvent my tool, which, instead of
 * helping me get my job done, is in the way. Sigh.
 *
 * Actually, C is even more retarded than I thought. I was going to check
 * the sizeof(cell) and set SH_CELL accordingly...but I can't do
 * "environmental queries" in the preprocessor! So the user gets to do this
 * by hand! Hooray for automation!
*/
void mu_cells(void)      { TOP <<= SH_CELL; }
void mu_cell_slash(void) { TOP >>= SH_CELL; }

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
    TOP = string_compare((char *)ST3, ST2, (char *)ST1, TOP);
    NIP(3);
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

void mu_cmove()
{
    void *src = (void *) ST2;
    void *dest = (void *) ST1;
    size_t count = TOP;

    memcpy(dest, src, count);
    DROP(3);
}
