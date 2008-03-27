/*
 * This file is part of muFORTH: http://pages.nimblemachines.com/muforth
 *
 * Copyright (c) 2002-2008 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* the very basic words */

#include "muforth.h"

#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

cell pop_dstack()  { cell t = TOP; DROP(1); return t; }

void mu_plus()  { TOP += ST1; NIP(1); }
void mu_and()   { TOP &= ST1; NIP(1); }
void mu_or()    { TOP |= ST1; NIP(1); }
void mu_xor()   { TOP ^= ST1; NIP(1); }

void mu_negate()  { TOP = -TOP; }
void mu_invert()  { TOP = ~TOP; }

void mu_2star()                    { TOP <<= 1; }
void mu_2slash()                   { TOP >>= 1; }
void mu_u2slash()  { TOP = (unsigned)TOP >>  1; }

void mu_shift_left()              { TOP = ST1 << TOP; NIP(1); }
void mu_shift_right()             { TOP = ST1 >> TOP; NIP(1); }
void mu_ushift_right()  { TOP = (unsigned)ST1 >> TOP; NIP(1); }

void mu_fetch()   { TOP =  *(cell *)TOP; }
void mu_cfetch()  { TOP = *(uint8 *)TOP; }

void mu_store()        { *(cell *)TOP = ST1; DROP(2); }
void mu_cstore()      { *(uint8 *)TOP = ST1; DROP(2); }
void mu_plus_store()  { *(cell *)TOP += ST1; DROP(2); }

void mu_dup()    { DUP; }
void mu_nip()    { NIP(1); }
void mu_drop()   { DROP(1); }
void mu_2drop()  { DROP(2); }
void mu_swap()   { cell t = TOP; TOP = ST1; ST1 = t; }
void mu_over()   { DUP; TOP = ST2; }          /* a b -> a b a */

void mu_rot()        { cell t = TOP; TOP = ST2; ST2 = ST1; ST1 = t; }
void mu_minus_rot()  { cell t = TOP; TOP = ST1; ST1 = ST2; ST2 = t; }

void mu_uless()  { TOP = (ST1 < (unsigned)TOP) ? -1 : 0; NIP(1); }
void mu_less()   { TOP = (ST1 < TOP)           ? -1 : 0; NIP(1); }

void mu_zero_less()   { TOP = (TOP <  0) ? -1 : 0; }
void mu_zero_equal()  { TOP = (TOP == 0) ? -1 : 0; }

void mu_depth()     { cell d = S0 - SP; PUSH(d); }
void mu_sp_reset()  { SP = S0; TOP = 0xdecafbad; }

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
 * So, I've given up on double-length math for muFORTH. It's a beautiful
 * and elegant part of Forth, but since I intend muFORTH mostly for
 * cross-compiling (to 32bit architectures at the moment, though that could
 * change!), single-length is plenty. So don't try using star-slash with
 * large operands. ;-)
 */

/*
 * We don't need a ustar, since single-length star and ustar yield the same
 * answers! (Prove this!)
 */
void mu_star()    { TOP *= ST1; NIP(1); }

void mu_uslash_mod()  /* u1 u2 -- um uq */
{
    ucell umod;
    ucell uquot;

    uquot = (unsigned)ST1 / TOP;
    umod  = (unsigned)ST1 % TOP;
    ST1 = umod;
    TOP = uquot;
}

/*
 * Of course, I'm not giving up floored division. ;-)
 *
 * Most processors do symmetric division. To fix this (to make it _FLOOR_)
 * we have to adjust the quotient and remainder when rem != 0 and the
 * divisor and dividend are different signs. (This is NOT the same as
 * quotient < 0, because the quotient could have been truncated to zero by
 * symmetric division when the actual (floored) quotient is < 0!) The
 * adjustment is:
 *
 *  quot_floored = quot_symm - 1
 *   mod_floored =  rem_symm + divisor
 *
 * This preserves the invariant a / b => (r,q) s.t. (q * b) + r = a.
 *
 *   (q' * b) + r' = (q - 1) * b + (r + b) = (q * b) - b + r + b
 *            = (q * b) + r
 *            = a
 *
 * where q',r' are the _floored_ quotient and remainder (really, modulus),
 * and q,r are the symmetric quotient and remainder.
 *
 */
void mu_slash_mod()  /* n1 n2 -- m q */
{
    cell mod;
    cell quot;

    quot = ST1 / TOP;
    mod  = ST1 % TOP;

#ifndef HOST_DIVIDE_FLOORS
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
void mu_cells(void)       { TOP <<= SH_CELL; }
void mu_cell_slash(void)  { TOP >>= SH_CELL; }

void mu_string_equal()   /* a1 len1 a2 len2 -- flag */
{
    if (ST2 != TOP)
        TOP = 0;            /* unequal if lengths differ */
    else
        TOP = (memcmp((char *)ST3, (char *)ST1, TOP) == 0) ? -1 : 0;

    NIP(3);
}

void mu_cmove()  /* src dest count */
{
    void *src = (void *) ST2;
    void *dest = (void *) ST1;
    size_t count = TOP;

    bcopy(src, dest, count);  /* allows overlapping strings */
    DROP(3);
}

#ifdef THIS_IS_SILLY
/*
 * I thought I wanted to be able to sort string, but I have more
 * interesting ideas about what muFORTH is good for. ;-)
 *
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
#endif
