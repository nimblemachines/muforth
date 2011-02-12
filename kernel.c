/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* the very basic words */

#include "muforth.h"

void mu_nope() {}    /* very useful NO-OP */
void mu_zzz()  {}    /* a convenient GDB breakpoint */

void mu_plus()  { ST1 += TOP; DROP(1); }
void mu_and()   { ST1 &= TOP; DROP(1); }
void mu_or()    { ST1 |= TOP; DROP(1); }
void mu_xor()   { ST1 ^= TOP; DROP(1); }

void mu_negate()  { TOP = -TOP; }
void mu_invert()  { TOP = ~TOP; }

void mu_2star()                 { TOP <<= 1; }
void mu_2slash()                { TOP >>= 1; }
void mu_u2slash()  { TOP = (uval) TOP >>  1; }

void mu_shift_left()           { ST1 = ST1 << TOP; DROP(1); }
void mu_shift_right()          { ST1 = ST1 >> TOP; DROP(1); }
void mu_ushift_right()  { ST1 = (uval) ST1 >> TOP; DROP(1); }

/* export cell size to Forth */
void mu_CELL_SHIFT()  { PUSH(CELL_SHIFT); }

/* fetch and store character (really _byte_) values */
void mu_cfetch()  { TOP = *(uint8_t *)TOP; }
void mu_cstore()  { *(uint8_t *)TOP = ST1; DROP(2); }

/* fetch and store cell values (32 or 64 bit) */
void mu_fetch()       { TOP =  *(cell *)TOP; }
void mu_store()       { *(cell *)TOP  = ST1; DROP(2); }
void mu_plus_store()  { *(cell *)TOP += ST1; DROP(2); }

/* copy nth value (counting from 0) to top - ANS calls this "pick" */
void mu_nth()    { TOP = SP[TOP+1]; }

void mu_dup()    { val t = TOP; PUSH(t); }
void mu_nip()    { val t = POP; TOP = t; }
void mu_drop()   { DROP(1); }
void mu_2drop()  { DROP(2); }
void mu_drops()  { DROP(TOP+1); }
void mu_swap()   { val t = TOP; TOP = ST1; ST1 = t; }
void mu_over()   { val o = ST1; PUSH(o); }          /* a b -> a b a */

void mu_uless()  { ST1 = (ST1 < (uval) TOP) ? -1 : 0; DROP(1); }
void mu_less()   { ST1 = (ST1 <        TOP) ? -1 : 0; DROP(1); }

void mu_zero_less()   { TOP = (TOP <  0) ? -1 : 0; }
void mu_zero_equal()  { TOP = (TOP == 0) ? -1 : 0; }

void mu_depth()     { cell d = S0 - SP; PUSH(d); }
void mu_sp_reset()  { SP = S0; SP[0] = 0xdecafbad; }
void mu_push_s0()   { PUSH_ADDR(S0); }          /* address of stack bottom */
void mu_sp_fetch()  { val *s = SP; PUSH_ADDR(s); } /* push stack pointer */
void mu_sp_store()  { SP = (val *)TOP; }           /* set stack pointer */

/* So we can do return-stack magic. */
void mu_rp_store()       { RP  = (val *)TOP; DROP(1); }
void mu_rp_plus_store()  { RP += TOP; DROP(1); }    /* TOP is cell count! */
void mu_rp_fetch()       { PUSH_ADDR(RP); }

/*
 * We don't need a ustar, since single-length star and ustar yield the same
 * answers! (Prove this!)
 */
void mu_star()    { ST1 *= TOP; DROP(1); }

void mu_uslash_mod()  /* u1 u2 -- um uq */
{
    uval umod;
    uval uquot;

    uquot = (uval)ST1 / TOP;
    umod  = (uval)ST1 % TOP;
    ST1 = umod;
    TOP = uquot;
}

/*
 * Even though I'm now allowing GCC its ugly machinations (so I can have
 * 64-bit multiply and divide), I'm not giving up floored division. ;-)
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
    val mod;
    val quot;

    quot = ST1 / TOP;
    mod  = ST1 % TOP;

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


void mu_string_equal()   /* a1 len1 a2 len2 -- flag */
{
    if (ST2 != TOP)
        ST3 = 0;            /* unequal if lengths differ */
    else
        ST3 = (memcmp((char *)ST3, (char *)ST1, TOP) == 0) ? -1 : 0;

    DROP(3);
}

void mu_cmove()  /* src dest count */
{
    void *src = (void *) ST2;
    void *dest = (void *) ST1;
    size_t count = TOP;

    memmove(dest, src, count);  /* allows overlapping strings */
    DROP(3);
}

/*
 * Since we're now re-allowing throw()ing of C-strings, we have to
 * calculate the length of a string when we want to print it out (in case
 * of error).
 */
/* stack: z" - z" count */
void mu_zcount()
{
    int len = strlen((char *)TOP);
    PUSH(len);
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
