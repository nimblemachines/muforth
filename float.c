/*
 * float.c
 *
 * This file implements floating point support for muForth.
 * As an implementation detail, the file can work in either
 * float's or double's.  Currently the option is a compile
 * time option.
 */
#include <stdio.h>
#include "muforth.h"

void mu_d_to_f(void)
{
	fpush(dpop());
}

void mu_f_to_d(void)
{
	dpush(fpop());
}

void mu_fadd(void)
{
	float_t a, b, c;

	a = fpop();
	b = fpop();
	c = a + b;
	fpush(c);
}

void mu_fneg(void)
{
	fpush(-fpop());
}

void mu_fmul(void)
{
	float_t a, b, c;

	a = fpop();
	b = fpop();
	c = a * b;
	fpush(c);
}

void mu_fdiv(void)
{
	float_t a, b, c;

	b = fpop();
	a = fpop();
	c = a / b;
	fpush(c);
}

/*
 * (f.)  (s f a max# -- a #)
 *
 * Given a buffer and size of the buffer (max#), this routine will print
 * f into that buffer and return the address of the buffer and the size of
 * string it put there.
 */
void mu_fdot(void)
{
	float_t n;
	char *buf;
	int   buf_size;

	buf_size = POP;
	buf = (char *) POP;
	n = fpop();

	snprintf(buf, buf_size, "%G", n);

	PUSH((cell_t) buf);
	PUSH(strlen(buf));
}

void mu_str_to_f(void)
{
	double df;
	char *buf;

	buf = (char *) POP;
	sscanf(buf, "%lf", &df);
	fpush(df);
}
