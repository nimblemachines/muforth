/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2005 David Frech. All rights reserved, and all wrongs
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

/* Thanks to Michael Pruemm for the idea of comparing RP to rp_saved as a
 * way to see when we're "done".
 */
static void run()
{
    xtk **rp_saved = RP;

    while (RP <= rp_saved)
        NEXT;
}

/* The most important "word" of all: */
void mu_do_colon() { NEST; IP = (xtk *)&W[1]; run(); }

/* The base of create/does>. */
void mu_do_does() { NEST; IP = (xtk *)W[1]; PUSH(W[2]); run(); }

void mu_exit()   { UNNEST; }

pw p_mu_exit = &mu_exit;

void mu_literal_() { PUSH(*IP++); }

cell mu_pop_dstack() { cell t = T; DROP; return t; }

void mu_add() { T += SND; NIP; }
void mu_and() { T &= SND; NIP; }
void mu_or()  { T |= SND; NIP; }
void mu_xor() { T ^= SND; NIP; }

void mu_negate() { T = -T; }
void mu_invert() { T = ~T; }

void mu_2star()               { T <<= 1; }
void mu_2slash()              { T >>= 1; }
void mu_u2slash()   { (unsigned)T >>= 1; }

void mu_shift_left()             { T = SND << T; NIP; }
void mu_shift_right()            { T = SND >> T; NIP; }
void mu_ushift_right() { T = (unsigned)SND >> T; NIP; }

void mu_fetch()  { T =    *(cell *) T; }
void mu_cfetch() { T = *(uint8_t *) T; }

void mu_store()       { *(cell *)T = SND; DROP2; }
void mu_cstore()   { *(uint8_t *)T = SND; DROP2; }
void mu_plus_store() { *(cell *)T += SND; DROP2; }

void mu_dup()  { DUP; }
void mu_nip()  { NIP; }
void mu_drop() { DROP; }
void mu_2drop() { DROP2; }
void mu_swap() { cell t = T; T = SND; SND = t; }
void mu_over() { DUP; T = TRD; }          /* a b -> a b a */

void mu_rot()       { cell t = T; T = TRD; TRD = SND; SND = t; }
void mu_minus_rot() { cell t = T; T = SND; SND = TRD; TRD = t; }

void mu_uless() { T = (SND < (unsigned) T) ? -1 : 0; NIP; }
void mu_less()  { T = (SND < T)            ? -1 : 0; NIP; }

void mu_zless()  { T = (T < 0)  ? -1 : 0; }
void mu_zequal() { T = (T == 0) ? -1 : 0; }

void mu_sp_fetch() { PUSH(SP); }
void mu_sp_store() { SP = (cell *) T; /* XXX: POP ? */ }

void mu_branch_()    { BRANCH; }
void mu_eqzbranch_() { if (T == 0) BRANCH; else IP++; }
void mu_zbranch_()   { mu_eqzbranch_(); DROP; }

/* r stack functions */
void mu_push()   { RPUSH(POP); }
void mu_pop()    { PUSH(RPOP); }
void mu_rfetch() { PUSH(RP[0]); }

void mu_next_()
{
    if (--(cell)RP[0] == 0)
    {
        IP += 1;  /* skip branch */
        RP += 1;  /* pop counter */
    }
    else
    {
        BRANCH;  /* take branch */
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
void mu_cells(void)      { T <<= SH_CELL; }
void mu_cell_slash(void) { T >>= SH_CELL; }

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
    T = string_compare((char *)SP[2], TRD, (char *)SND, T);
    NIPN(3);
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
    void *src = (void *) TRD;
    void *dest = (void *) SND;
    size_t count = T;

    memcpy(dest, src, count);
    DROPN(3);
}
