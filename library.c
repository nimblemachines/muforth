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

/*
 * fpop() and fpush()
 *
 * These routines are implemented in C and must be hand-coded in
 * assembler for your target machine, if you chose to use floating point.
 * An entire muForth system can be built without floating point, so don't
 * think you need it.
 *
 * Also these routines aren't in the mu_xyz() name space because they
 * aren't available to muForth programmers directly.  They are used
 * indirectly to perform work on behalf of routines that muForth code
 * may invoke.
 *
 * I added the bcopy() to put and fetch the float from the stack because
 * on some processor architectures doubles need to be aligned naturally.
 * Since the Forth stack cannot be coerced into such alignment rules, we've
 * got trouble.
 */
float_t fpop(void)
{
	float_t *p, f;
	
	p = (float_t *) sp;
	bcopy(p, &f, sizeof(f));

	sp += FLOAT_NCELLS;

	return f;
}

void fpush(float_t f)
{
	float_t *p;

	sp -= FLOAT_NCELLS;

	p = (float_t *) sp;
	bcopy(&f, p, sizeof(f));
}

/*
 * dpop() and dpush()
 *
 * Double cells are necessary for muForth.  They are used in
 * division and multiplication.  Division uses dcells.
 */
dcell_t dpop(void)
{
	dcell_t r;
	ucell_t lo;
	cell_t hi;

	hi = POP;
	lo = POP;
	r = hi;
	r <<= 32;
	r |= lo;

	return r;
}

void dpush(dcell_t d)
{
	ucell_t lo;
	cell_t hi;
	hi = d >> 32;
	lo = d & 0xFFFFFFFF;
	PUSH(lo);
	PUSH(hi);
}

void mu_dnegate(void)
{
#if 1
	cell_t lo, hi;

	DPOP(lo, hi);

	if (lo == 0)
		DPUSH(-lo, -hi);
	else
		DPUSH(-lo, ~hi);
#else
	/*
	 * The other way to implement this is:
	 */
	dcell_t n;
	n = dpop();
	dpush(-n);
	/*
	 * The reason this isn't the implementation is to show
	 * that the simpler logic, above, is quite valid.
	 * Given that, it should make it easier to carry forward
	 * to a muForth implementation on your target machine.
	 */
#endif
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

