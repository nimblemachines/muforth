/*
 * library.c
 *
 * This file is meant to supply the library functions for performing
 * non-kernel muforth-isms.  For example, the math routines that
 * implement division and multiplication are here.
 * 
 * Also the double words are present: dadd, dnegate.
 *
 * Etc.
 */
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "muforth.h"
#include "opcode.h"

#define DPOP(lo, hi)	(hi = POP, lo = POP)
#define DPUSH(lo, hi)	(PUSH(lo), PUSH(hi))
void mu_dnegate(void)
{
	cell_t lo, hi;

	DPOP(lo, hi);

	if (lo == 0)
		DPUSH(-lo, -hi);
	else
		DPUSH(-lo, ~hi);
}

void mu_dplus(void)
{
	cell_t a_lo, a_hi;
	cell_t b_lo, b_hi;
	cell_t r_lo, r_hi;

	DPOP(a_lo, a_hi);
	DPOP(b_lo, b_hi);

	r_lo = a_lo + b_lo;
	r_hi = a_hi + b_hi;
	if (r_lo < a_lo)
		r_hi ++;

	DPUSH(r_lo, r_hi);
}

void mu_fetch_literal_value(void)
{
	cell_t *ip;

	ip = (cell_t *) POP;
	assert (*ip == LIT_LOAD || *ip == LITERAL);
	PUSH(ip[1]);
}

static int is_division_floored(void)
{
	static int tested;
	static int division_type;

	if (tested) return division_type;

	/*
	 * David Frech wrote that to fix symmetric division, we must:
	 *	 floor_quot = symm_quot - 1
         *	 mod = rem + divisor
	 */

	if (-2/3 == 0) {
		/*
		 * it's symmetric
		 */
		division_type = 0;
	} else {
		assert(-2/3 == -1);
		division_type = 1;
	}
	tested = 1;

	return division_type;
}

static int signbit(long int n)
{
	return (n < 0) ? 1 : 0;
}

void mu_fm_slash_mod(void)
{
	int divisor;
	long long dividend;
	int q, r;

	divisor = POP;
	dividend = POP; /* dividend hi */
	dividend <<= sizeof(unsigned int) * 8;
	dividend |= (unsigned int) POP;

	q = dividend / divisor;
	r = dividend - (q * divisor);

	if (!is_division_floored() && r != 0 && signbit(dividend) != signbit(divisor)) {
		q -= 1;
		r += divisor;
	};
	PUSH(r);
	PUSH(q);
}

void mu_um_slash_mod(void)
{
	unsigned long long dividend;
	unsigned long divisor;
	cell_t q, r;

	divisor = POP;
	dividend = POP; /* dividend hi */
	dividend <<= sizeof(unsigned int) * 8;
	dividend |= (unsigned int) POP;

	q = dividend / divisor;
	r = dividend - (q * divisor);

	PUSH(r);
	PUSH(q);
}

void mu_m_star(void)
{
	int a, b;
	long long r;

	b = POP;
	a = POP;

	r = a * b;

	DPUSH( r & 0xFFFFFFFF, r >> 32);
}

void mu_um_star(void)
{
	unsigned long long r;
	unsigned long long a, b;

	a = POP;
	b = POP;

	r = a * b;

	DPUSH( r & 0xFFFFFFFF, r >> 32);
}

