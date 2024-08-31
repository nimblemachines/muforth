/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* the very basic words */

#include "muforth.h"

void mu_nope()  {}    /* very useful NO-OP */
void mu_zzz()   {}    /* a convenient GDB breakpoint */

void mu_plus()  { ST1 += TOP; DROP(1); }
void mu_and()   { ST1 &= TOP; DROP(1); }
void mu_or()    { ST1 |= TOP; DROP(1); }
void mu_xor()   { ST1 ^= TOP; DROP(1); }

void mu_negate()    { TOP = -TOP; }
void mu_invert()    { TOP = ~TOP; }

void mu_2star()                   { TOP <<= 1; }
void mu_2slash()                  { TOP >>= 1; }
void mu_u2slash()   { TOP = (ucell) TOP >>  1; }

/*
 * Representation of truth values
 *
 * In C, booleans are represent by integers. Zero means false, and anything
 * non-zero means true. But the value returned by a C comparison operator -
 * "<" for example - is always 0 or 1.
 *
 * Forth works similarly, and in its earliest days, comparison words like
 * "<" or "0=" returned 0 for false and 1 for true. But starting in 1983 or
 * so this was changed. The "standard" way now, in Forth, is to return -1
 * (all ones) for true. This value is more useful, and it can be ANDed with
 * other values to produce useful results without needing to jump.
 *
 * In the following code you will see the arithmetic negation ("-") of
 * several comparison operators. This simply converts the C true (1) to the
 * Forth true (-1), leaving false alone. It's not obvious, but it *is*
 * simple and succint.
 */

/*
 * Let's give very specific semantics to left and right shifts, rather than
 * leaving their semantics to the whims of C's underspecified nature.
 *
 * Shift amounts strictly less than (sizeof(cell) * 8) will be shifted
 * "normally" via C's << or >> operator.
 *
 * Shift amounts greater than or equal to (sizeof(cell) * 8) - let's call
 * these "big" shifts - will instead result in all ones or all zeros,
 * depending on the type of shift, and the value shifted. In particular:
 *
 *  - Big left shifts and big unsigned right shifts of any value result in all zeros.
 *  - Big signed right shifts of *non-negative* values result in all zeros.
 *  - Big signed right shifts of *negative* values result in all ones.
 */
#define BIGSHIFT  (TOP >= (sizeof(cell) * 8))
#define SIGN(x)   -((x) < 0)

void mu_shift_left()    { ST1 = BIGSHIFT ? 0 :         ST1 << TOP; DROP(1); }
void mu_ushift_right()  { ST1 = BIGSHIFT ? 0 : (ucell) ST1 >> TOP; DROP(1); }
void mu_shift_right()   { ST1 = BIGSHIFT ? SIGN(ST1) : ST1 >> TOP; DROP(1); }

/* A cell is the unit of storage in the Forth heap. A cell is 64 bits. */
void mu_cells()         { TOP <<= 3; }
void mu_cell_slash()    { TOP >>= 3; }   /* signed & flooring! */

/* fetch and store character (really unsigned byte) values */
void mu_cfetch()        { TOP = *(uint8_t *)UNHEAPIFY(TOP); }
void mu_cstore()        { *(uint8_t *)UNHEAPIFY(TOP) = ST1; DROP(2); }

/* fetch and store cell values */
void mu_fetch()         { TOP = *UNHEAPIFY(TOP); }
void mu_store()         { *UNHEAPIFY(TOP)  = ST1; DROP(2); }
void mu_plus_store()    { *UNHEAPIFY(TOP) += ST1; DROP(2); }

/* copy nth value (counting from 0) to top - ANS calls this "pick" */
void mu_nth()       { TOP = SP[TOP+1]; }

/* t means TOP, s means SECOND */
void mu_dup()       { cell t = TOP; PUSH(t); }              /* a   -> a   a */
void mu_over()      { cell s = ST1; PUSH(s); }              /* a b -> a b a */
void mu_swap()      { cell t = TOP; TOP = ST1; ST1 = t; }   /* a b ->   b a */

void mu_drop()      { DROP(1); }
void mu_2drop()     { DROP(2); }
void mu_drops()     { DROP(TOP+1); }

void mu_uless()     { ST1 = -(ST1 < (ucell) TOP); DROP(1); }
void mu_less()      { ST1 = -(ST1 <         TOP); DROP(1); }

void mu_0less()     { TOP = -(TOP <  0); }
void mu_0equal()    { TOP = -(TOP == 0); }

void mu_depth()     { int d = SP0 - SP; PUSH(d); }
void mu_sp_reset()  { SP = SP0; SP[0] = 0xdecafbad; }

#ifdef WITH_SP_OPERATORS
void mu_push_s0()   { PUSH_ADDR(SP0); }         /* address of stack bottom */
void mu_sp_fetch()  { cell *s = SP; PUSH_ADDR(s); }     /* push stack pointer */
void mu_sp_store()  { SP = (cell *)TOP; }               /* set stack pointer */
#endif

/* So we can do return-stack magic. */
void mu_runtime_rp_fetch()       { PUSH_ADDR(RP); }
void mu_runtime_rp_store()       { RP  = UNHEAPIFY(TOP); DROP(1); }
void mu_runtime_rp_plus_store()  { RP += TOP; DROP(1); }    /* TOP is cell count! */

/*
 * We don't need a ustar, since single-length star and ustar yield the same
 * answers! (Prove this!)
 */
void mu_star()    { ST1 *= TOP; DROP(1); }

void mu_uslash_mod()  /* u1 u2 - um uq */
{
    ucell uquot = (ucell)ST1 / TOP;
    ucell umod  = (ucell)ST1 % TOP;
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
void mu_slash_mod()  /* n1 n2 - m q */
{
    cell quot = ST1 / TOP;
    cell mod  = ST1 % TOP;

#ifdef MU_DIVISION_IS_SYMMETRIC
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

void mu_string_equal()   /* a1 len1 a2 len2 - flag */
{
    if (ST2 != TOP)
        ST3 = 0;            /* unequal if lengths differ */
    else
        ST3 = (memcmp((char *)UNHEAPIFY(ST3),
                      (char *)UNHEAPIFY(ST1), TOP) == 0) ? -1 : 0;

    DROP(3);
}

void mu_cmove()  /* src dest count */
{
    void *src = (void *)UNHEAPIFY(ST2);
    void *dest = (void *)UNHEAPIFY(ST1);
    size_t count = TOP;

    memmove(dest, src, count);  /* allows overlapping strings */
    DROP(3);
}

/* Fill count bytes at dest with bytes equal to value. */
void mu_fill()  /* dest count value */
{
    void *dest = (void *)UNHEAPIFY(ST2);
    size_t count = ST1;
    int value = TOP;

    memset(dest, value, count);
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
    int len = strlen((char *)UNHEAPIFY(TOP));
    PUSH(len);
}

#ifdef THIS_IS_SILLY
/*
 * XXX If this is ever used, string addresses on the stack need to be
 * UNHEAPIFY'd.
 *
 * I thought I wanted to be able to sort strings, but I have more
 * interesting ideas about what muforth is good for. ;-)
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
