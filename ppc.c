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
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

/* compiler essentials for Power PC architecture */

#include "muforth.h"

#ifdef PPC_TEST
u_int8_t *pnm0, *pcd0, *pdt0;	   /* ptrs to name, code, & data spaces */
u_int8_t *pnm, *pcd, *pdt;	/* ptrs to next free byte in each space */
/* data stack */
int stack[STACK_SIZE];
int *sp = S0;
#endif /* PPC_TEST */

/*******************************************************************
 *
 * The PowerPC is a 32 bit processor with 32 bit instructions.
 * This file works completely with 32 bit entities.  (Except where
 * two 32 bit entities are used in conjunction to build a 64 bit
 * entity.)
 */

/*******************************************************************
 *
 * There are two kinds of externally visible functions in this file:
 * 1) those that build code in the dictionary, i.e., .compiler. words
 * 2) those that perform run-time functionality, i.e., .forth. words
 */
static u_int32_t *pcd_last_call;

/************************************************************************
 *
 * PPC Instructions
 * 
 * I chose not to embed instruction opcodes directly into the C code
 * but instead defined them here.  Hopefully this helps to code to be
 * readable.
 */
#define INSTR_MASK	0xFC000000

/*
 * BRANCHES
 *
 * Branch And Link (BAL)
 * The Link register is a special processor register which holds the
 * return address.  A called routine can branch to the link register
 * to return to the caller.  Routines that call other routines must
 * preserve the Link register.  Often a stack is employed for this
 * purpose.  The BAL instruction has two low order bits.  One indicates
 * whether the return address is placed into the Link register.  The
 * other indicates if the address supplied is relative or absolute.
 */
#define BR		0x48000000
#define BRCOND		0x40000000
#define BR_AND_LINK	1
#define BR_ABS_ADDR	2
#define	BAL		(BR | BR_AND_LINK)
#define BR_COND_ZERO	(BRCOND | RS(12))

/*
 * BLR_ALWAYS is Branch And Link to Link Register, Always
 *
 * After using the Link Register as the destination of the branch, the
 * address of the instruction following the branch is placed into the
 * Link Register.
 */
#define BLRL_ALWAYS	0x4e800021
#define BLR_ALWAYS	0x4e800020

/*
 * LOAD STORE OPS
 *
 * The Load and Store operations use RS (as defined here, see below)
 * as the data register and use RA as the address of the operation.
 * LD/ST/STU have 16 bit, sign-extended offsets in bits 0..15.
 */
#define LD		0x80000000
#define ST		0x90000000
#define STU		0x94000000

/*
 * Move To Link Register
 * This is actualy, move to special purpose register, lr.  Yeah, yeah.
 * When you use this you MUST specify a register to use, too.
 */
#define MTLR		0x7c0803a6
#define MFLR		0x7c0802a6


#define SHIFT_LEFT	0x7C000030

/*
 * Register definitions:
 *
 * Temp is just that
 * PUSH_TEMP is different.  Usually, it would just be Temp.
 * But, note that it is used as part of the relationship between
 * BUILD and DOES> .  BUILD will set the PUSH_TEMP register with
 * the top of stack after the BUILD is complete.  Then, when DOES>
 * is executed, the first thing it will do is to push this value
 * onto the stack.  See mu_compile_literal_load() and 
 */
#define R_TEMP		3
#define R_A		4
#define R_B		5
#define R_C		6
#define R_D		7
#define R_PUSH_TEMP	11
#define R_SP		15
#define R_RP		1
#define NUM_REGS	32

/*
 * INTEGER OPS
 *
 * Integer insructions have the basic form of:
 *     <OP> <RS> <RA> <RB>
 * or  <OP> <RS> <RA> <immediate value>
 *
 * In either case, RA is the destination, RS and, optionally, RB
 * are inputs to the operation.
 */
#define RS_SHIFT	21
#define RA_SHIFT	16
#define RB_SHIFT	11

#define RS(r)		((r) << RS_SHIFT)
#define RA(r)		((r) << RA_SHIFT)
#define RB(r)		((r) << RB_SHIFT)

#define IMM_MASK	0x0000FFFF
#define HI_MASK		0xFFFF0000
#define IMM_SIGN	0x00008000

/*
 * The register convention appears to be:
 * See: e-www.motorola.com/files/ 32bit/doc/ref_manual/E500ABIUG.pdf
 * R0-R31 are the registers:
 * R0       - Volatile, used during linkage
 * R1       - Dedicated, Stack Frame Pointer
 * R2       - Reserved
 * R3..R10  - Passed parameters to called functions.
 * R3..R4   - Return values from functions
 * R11..R12 - Volatile, used during linkage
 * R13      - Dedicated, small data area pointer
 * R14..R31 - Nonvolatile, used for local variables
 *
 * This version uses the stack frame pointer for
 * the return stack.
 *
 * Also, R14 contains a pointer to the global variable sp.
 * This is good for two reasons.  First, Forth words don't have
 * to generate code to load the address of the stack pointer
 * (2 instructions) and C code won't tromple on top of it.
 * The reason for this is that R14 is a nonvolatile register
 * and thus the compiler will generate code to preserve R14
 * before munging it and will reserect it before returning.
 * However, if a C routine called a Forth word and that
 * C routine had pushed R14 and then overwritten it, then
 * the Forth word would execute without the benefit of R14.
 * Hmmm.  Is it possible for C functions to execute Forth
 * routines?  How does this system get started?  What's
 * the transition between C and Forth?
 */

/*
 * ADD Immediate is the way to load a value into a register.  This
 * instruction form must use R0 as the RS register.  In this very
 * specific case, R0 has the value 0.
 * The ADDI instruction will load 16 bits of data into the low order
 * bits of the register.  ADDI Shifted will load 16 bits of data into
 * the hi order bits of the register.
 *
 * NOTE 1: R0 does -NOT- often take on the value 0.  Just with this one instruction.
 * NOTE 2: The immediate value is sign extended.  This means that if bit 15
 *         of the value to be loaded is set, the high order data has to have
 *         one added to it.
 */
#define ADD		0x7C000214
#define ADDI		0x38000000
#define ADDIS		0x3C000000

#define CMPI		0x2C000000

/*
 * Cacheline Size
 *
 * This is a nasty constant.  It determines how big the code believes
 * the cacheline of the processor is.  Really, this isn't a constant
 * it's dependent upon the processor architecture.  For right now,
 * I'm hardcoding it to 32 bytes.
 */
#define CACHELINE_SIZE		32



/**************************************************
 *
 * HELPER FUNCTIONS
 *
 */

/*
 * mu_iflush_addr();
 *
 * This routine is defined in assembly language (see ppc_asm.s).
 * It will flush a cache block from the d-cache and i-cache.
 */
extern void mu_iflush_addr(void *addr);

/*
 * comma_instr(int instr)
 *
 * This routine will append the instruction into the instruction stream.
 * And, it flushes the instruction from the data stream.
 */
static void comma_instr(int instr)
{
	u_int32_t *pc = (u_int32_t *) pcd;

	*pc = instr;

	mu_iflush_addr(pc);

	pcd += 4;
}

/*****************************************************************
 *
 * comma_xyz()
 *
 * Here are a bunch of functions which are used for code generation.
 * The reason I used these functions instead of macros is a matter
 * of taste.  I have more control over the generation of code with
 * functions than with macros, I believe.
 *
 * I wanted a central routine to perform these ops so that if I ever
 * discover a bug with any of them, I don't have to change code
 * everywhere but instead in centralized routines.
 */
/*
 * assert_valid_reg()
 */
static void assert_valid_reg(unsigned int reg)
{
	assert(reg < NUM_REGS);
}

static void assert_16bit_val(unsigned int val)
{
	unsigned int hi = val & HI_MASK;
	if (hi) {
		assert(hi == HI_MASK);
	}
}

static void comma_addi(int reg, int val)
{
	assert_valid_reg(reg);

	comma_instr(ADDI | RS(reg) | RA(reg) | (val & IMM_MASK));
}

static void comma_ld(int r_dest, unsigned int offset, int r_base)
{
	assert_valid_reg(r_dest);
	assert_valid_reg(r_base);
	assert_16bit_val(offset);

	comma_instr(LD | RS(r_dest) | RA(r_base) | (offset & IMM_MASK));
}

static void comma_st(int r_src, int offset, int r_base)
{
	assert_valid_reg(r_src);
	assert_valid_reg(r_base);
	assert_16bit_val(offset);

	comma_instr(ST | RS(r_src) | RA(r_base) | (offset & IMM_MASK));
}

static void comma_stu(int r_src, int offset, int r_base)
{
	assert_valid_reg(r_src);
	assert_valid_reg(r_base);
	assert_16bit_val(offset);

	comma_instr(STU | RS(r_src) | RA(r_base) | (offset & IMM_MASK));
}

static void comma_cmpi(int r, int value)
{
	assert_valid_reg(r);
	assert_16bit_val(value);

	comma_instr(CMPI | RA(r) | value);
}

static void comma_li(int r, int val)
{
	u_int32_t lo, hi;

	assert_valid_reg(r);
	assert(r != 0);

	lo = (val >>  0) & IMM_MASK;
	hi = (val >> 16) & IMM_MASK;

	if (lo & IMM_SIGN) {
		hi += 1;
		hi &= IMM_MASK;
	}

	assert_16bit_val(lo);
	assert_16bit_val(hi);

	if (lo) {
		comma_instr(ADDI | RS(r) | RA(0) | lo);
		if (hi) 
			comma_instr(ADDIS | RS(r) | RA(r) | hi);
	} else {
		if (hi)
			comma_instr(ADDIS | RS(r) | RA(0) | hi);
		else
			comma_instr(ADDI | RS(r) | RA(0) | 0);
	}
}

static void comma_mtlr(int r)
{
	comma_instr(MTLR | RS(r));
}

static void comma_mflr(int r)
{
	comma_instr(MFLR | RS(r));
}

static void comma_pop(int r, int r_sp)
{
	comma_ld(r, 0, r_sp);
	comma_addi(r_sp, 4);
}

static void comma_push(int r, int r_sp)
{
	comma_stu(r, -4, r_sp);
}

/*
    IDIV is symmetric.  To fix it (to make it _FLOOR_) we have to
    adjust the quotient and remainder when BOTH rem /= 0 AND
    the divisor and dividend are different signs.  (This is NOT the
    same as quot < 0, because the quot could be truncated to zero
    by symmetric division when the actual quotient is < 0!)
    The adjustment is q' = q - 1; r' = r + divisor.

    This preserves the invariant a / b => (r,q) s.t. qb + r = a.

    q'b + r' = (q - 1)b + (r + b) = qb - b + r + b
             = qb + r
             = a

    where q',r' are the _floored_ quotient and remainder (really, modulus),
    and q,r are the symmetric quotient and remainder.

    XXX: discuss how the range of the rem/mod changes as the num changes
    (in symm. div.) and as the denom changes (in floored div).
*/

void mu_fm_slash_mod()		/* floored division! */
{
	u_int32_t divisor;
	int64_t dividend;
	u_int32_t rem, q;

	divisor = POP;

	dividend = POP;
	dividend <<= 32;
	dividend += POP;

	q = dividend / divisor;
	rem = dividend - (divisor * q);

	PUSH(rem);
	PUSH(q);
}

/*
 * mu_compile_call()
 *
 * The address of the entity to call is the top element of the stack.
 * This routine will place into the dictionary heap a call instruction
 * to that address.
 *
 * And, if this call is to C code, then reload the Stack Pointer (R15).
 */
void mu_compile_call()
{
	u_int32_t instr, addr;

	pcd_last_call = (u_int32_t *) pcd;

	addr = POP;

	if (addr & 0x00000003) {
		/*
		 * ERROR: We cannot handle calling a function at an address
		 * that is not 4 byte aligned.
		 */

		fprintf(stdout, "ERROR: an attempt to call a function that is "
			"not 4 byte aligned!\n");

		exit(-1);
	}

	/*
	 * If we will call a C function, we need to write the SP register back to memory
	 */
	if ((u_int8_t *) addr < pcd0)
	{
		comma_st(R_SP, 0, 14);
		comma_addi(R_RP, -16);
	}

	if ((addr == 0) || (addr & 0xFC000000)) {
		/*
		 * The address does not fit into a branch instruction.
		 * The address must be loaded into a temporary register
		 * so we can jump through it.
		 */
		comma_li(R_TEMP, addr);
		comma_mtlr(R_TEMP);
		comma_instr(BLRL_ALWAYS);
	} else {
		/*
		 * The address is OK to jump to directly
		 */
		instr = BAL | addr | BR_ABS_ADDR;

		/*
		 * Place the instruction in memory
		 */
		comma_instr(instr);
	}

	/*
	 * If we called a C function, we need to reload the SP register
	 */
	if ((u_int8_t *) addr < pcd0)
	{
		comma_ld(R_SP, 0, 14);
	}
}

/* resolve a forward or backward jump - moved from startup.mu4 because it
 * was i386-specific. In this usage "src" points just _past_ the displacement
 * we need to fix up.
 * : resolve   ( src dest)  over -  swap cell- ! ( 32-bit displacement) ;
 */
void mu_resolve(void)
{
}

void mu_compile_literal_load()
{
	comma_li(R_PUSH_TEMP, POP);
}

void mu_compile_literal_push()
{
	comma_push(R_PUSH_TEMP, R_SP);
}

void mu_compile_drop(void)
{
	comma_addi(R_SP, 4);
}

void mu_compile_2drop(void)
{
	comma_addi(R_SP, 4);
}

void mu_compile_entry(void)
{
	comma_mflr(R_TEMP);
	comma_push(R_TEMP, R_RP);
}

void mu_compile_exit(void)
{
	comma_pop(R_TEMP, R_RP);
	comma_mtlr(R_TEMP);
	comma_instr(BLR_ALWAYS);
}

/*
 * mu_compile_shunt()
 *
 * Drop 4 bytes from r stack
 */
void mu_compile_shunt()		/* drop 4 bytes from r stack */
{
	comma_addi(R_RP, 4);
}

void mu_compile_pop_from_r()
{
	comma_pop(R_TEMP, R_RP);
	comma_push(R_TEMP, R_SP);
}

void mu_compile_2pop_from_r()
{
	/*
	 * This could be optimized, later.
	 */
	comma_pop(R_TEMP, R_RP);
	comma_push(R_TEMP, R_SP);
	comma_pop(R_TEMP, R_RP);
	comma_push(R_TEMP, R_SP);
}

void mu_compile_copy_from_r()
{
	comma_ld(R_TEMP, 0, R_RP);
	comma_push(R_TEMP, R_SP);
}

void mu_compile_push_to_r()
{
	comma_pop(R_TEMP, R_SP);
	comma_push(R_TEMP, R_RP);
}

void mu_compile_2push_to_r()
{
	/*
	 * This could be optimized, later.
	 */
	comma_pop(R_TEMP, R_SP);
	comma_push(R_TEMP, R_RP);
	comma_pop(R_TEMP, R_SP);
	comma_push(R_TEMP, R_RP);
}

void mu_dnegate(void)
{
	u_int64_t *dsp;

	dsp = (u_int64_t *) sp;

	*dsp = - *dsp;
}

void mu_dplus(void)
{
	u_int64_t *dsp, a, b, c;

	dsp = (u_int64_t *) sp;

	a = dsp[0];
	b = dsp[1];

	c = a + b;

	dsp[1] = c;

	sp += 4;
}

/*
 * mu_um_star()
 * mu_m_star()
 *
 * The top two cells on the stack are multiplied together and
 * replaced with the two-cell result.  The high order of the
 * result is on top, with the low order of the result below it.
 *
 * um == Unsigned Multiply
 * m  == Signed Multiply
 */
void mu_um_star()
{
	u_int32_t a, b;
	u_int64_t r;

	a = POP;
	b = POP;

	r = a * b;

	PUSH(r);
	PUSH(r >> 32);
}

void mu_m_star()
{
	u_int32_t a, b;
	int64_t r;

	a = POP;
	b = POP;

	r = a * b;

	PUSH(r);
	PUSH(r >> 32);
}

/*
 * mu_um_slash_mod()
 * 
 * Given the top three stack elements a, b, and c, where a is the top element.
 * The two elements b and c are interpreted as being a single DCELL number.
 * The b:c pair are then divided by a.  The results of this function are
 * the remainder and quotient.  The quotient will be the top element of the
 * stack.
 * 
 * With a / b, a is the dividend and b is the divisor.  The quotient is the
 * integer result of division.  The remainder is the difference between
 * the dividend and the quotient times the divisor.
 * Example: 7 / 3 = quotient 2, remainder 1.
 * 
 * :  um_slash_mod  ( dividend-lo divident-hi  divisor --  remainder  quotient )
 * 
 */
void mu_um_slash_mod()
{
	u_int32_t divisor;
	u_int64_t dividend;
	u_int32_t rem, q;

	divisor = POP;

	dividend = POP;
	dividend <<= 32;
	dividend += POP;

	q = dividend / divisor;
	rem = dividend - (divisor * q);

	PUSH(rem);
	PUSH(q);
}

/*
 * mu_jump()
 *
 * The Link Register contains a pointer to the first word following the
 * jump FORTH word.  
 *
 * NOTE: "The" bug with the following code is that it assumes that it gets
 * called.  That is, it uses the Link Register as though it were called.
 * However, it's not, the followign code will appeear in-line.  Therefore,
 * either, the code has to be modified to set the link register itself, or
 * it has to be designed to not require this.
 */
u_int32_t mu_jump_helper(u_int32_t *base)
{
	int index;
	u_int32_t word;

	index = POP;
	word = base[index];
	assert((word & INSTR_MASK) == BR);
	assert((word & (BR_AND_LINK | BR_ABS_ADDR)) == (BR_AND_LINK | BR_ABS_ADDR));
	word &= ~(INSTR_MASK | BR_AND_LINK | BR_ABS_ADDR);

	return word;
}

void mu_compile_nondestructive_zbranch(void)
{
	comma_ld(R_TEMP, 0, R_SP);
	comma_cmpi(R_TEMP, 0);
	comma_instr(BR_COND_ZERO);
	PUSH(pcd);
}

void mu_compile_destructive_zbranch(void)
{
	comma_pop(R_TEMP, R_SP);
	comma_cmpi(R_TEMP, 0);
	comma_instr(BR_COND_ZERO);
	PUSH(pcd);
}

void mu_compile_branch(void)
{
	comma_instr(BR);
	PUSH(pcd);			/* offset to fix up later */
}

/*
 * mu_fetch_literal_value()
 *
 * The top of stack points to the beginning of a load immediate
 * code sequence.  The first instruction will be an ADDI or an
 * ADDIS, but to note that the RS instruction will be R_PUSH_TEMP and
 * the RA field will be R0 (anything else is an error).
 * Then, if the first instruction is an ADDI, and if the next instruction
 * is an ADDIS and if the next instruction's RS field is R_PUSH_TEMP, then
 * the next instruction's RA field must be R_PUSH_TEMP, too.  (Else, what
 * was the purpose of the previous instruction?)
 * Then, the full literal value can be extracted by either the one
 * instruction or both instructions.
 */
void mu_fetch_literal_value(void)
{
	u_int32_t *addr, val, hi;
	u_int32_t instr;

	addr = (u_int32_t *) POP;

	instr = addr[0];

	assert((instr & RS(31)) == RS(R_PUSH_TEMP));
	assert((instr & RA(31)) == RA(0));

	switch (instr & INSTR_MASK) {
	case ADDI:
		val = instr & IMM_MASK;
		if (val & IMM_SIGN) {
			val |= HI_MASK;
		}
		instr = addr[1];
		if (((instr & INSTR_MASK) == ADDIS) &&
		    ((instr & RS(31)) == RS(R_PUSH_TEMP)) &&
		    ((instr & RA(31)) == RA(R_PUSH_TEMP))) {
			hi = (instr & IMM_MASK) << 16;
			val += hi;
		}
		break;

	case ADDIS:
		val = (instr & IMM_MASK) << 16;
		break;

	default:
		fprintf(stderr, "ERROR: fetch_literal_value() points to bogus instruction stream!\n");
		exit (-1);
	}

	PUSH(val);
}

/*
 * mu_compile_qfor()
 *
 * The basic idea behind the for loop construct is that it takes only one
 * parameter which is the number of times to loop.  for will loop 4+ Billion
 * times if the input count is 0.  ?for will not loop at all if the input
 * count is 0.  One gets access to the count by using r@.  I'd prefer that
 * i be the word to fetch this value.  In that way the PPC version could use
 * the CTR register for loops.  And, the value on the Return stack that is
 * consumed by the current count on the i386 would have the old CTR register
 * value.  This would allow nexted loops and it would have the same stack
 * shape, though the contents would be different.  With i, then, the user
 * code wouldn't change and would work correctly in both environments.
 * In i386 i would perform r@ and on PPC i would copy from the PowerPC special
 * purpose register CTR.
 *
 * Here's the definition of for, ?for, and next:
 *   : for     ( - dest)      \ push  \ begin ;
 *   : ?for    ( - src dest)  (?for) ;
 *   : next    ( dest)        (next)  <resolve ;
 *
 * It looks to me like for and ?for leave different turds on the stack.
 * for leaves just the begin loop address.  Whereas, ?for leaves a begin
 * loop address -and- a branch if zer0 loop address.  Therefore, one
 * would write:
 *
 *	for ... next
 * 
 * Whereas with ?for, one must write:
 *
 *	?for ... next then
 *
 * And, in fact, through inspection of the usage of ?for, that's exactly
 * what I find in muforth.
 */
void mu_compile_qfor(void)
{
	comma_pop(R_TEMP, R_SP);	/* R_TEMP = *R_SP ++; */
	comma_cmpi(R_TEMP, 0);		/* z = (R_TEMP == 0); */
	comma_instr(BR_COND_ZERO);	/* if (!z) { */
	PUSH(pcd);			/* Save the address of the branch */
	comma_push(R_TEMP, R_RP);	/*	*--R_SP = R_TEMP; */
	PUSH(pcd);			/* push address to loop back to */
}

/*
 * mu_compile_next()
 *
 * The top of the return stack contains the loop counter.
 * Decrement it and branch (backwards) if not zero.  If it
 * is zero, pop the item off the stack and return.
 */
void mu_compile_next(void)
{
	comma_ld(R_TEMP, 0, R_RP);	/* R_TEMP = *R_RP; */
	comma_cmpi(R_TEMP, 0);		/* z = (R_TEMP == 0); */
	comma_instr(BR_COND_ZERO);	/* } while (!z); */
	PUSH(pcd);			/* Compiler push of the branch address */
	comma_addi(R_RP, 4);		/* pop the count off the return stack */
}

/************************************************************
 *
 * TEST CODE
 *
 * The following code is used if ppctest is made.  It allows
 * a simple, standalone version of this file (and ppc_asm.s).
 *
 * With this code, one can test the code generation.
 */

#ifdef PPC_TEST
void func_test(void)
{
	printf("Hello world!  POP = %8.8x\n", POP);
}

void test_comp_fetch_lit(unsigned int n)
{
	int b;

	PUSH(pcd);

	PUSH(n);
	mu_compile_literal_load();
	mu_compile_literal_push();
	mu_fetch_literal_value();

	b = POP;
	if (b != n) {
		printf("pushed 0x%8.8x, fetched 0x%8.8x\n", n, b);
		assert(n == b);
	}
	PUSH(func_test);
	mu_compile_call();
}

int main(void)
{
	pcd0 = malloc(1024);
	pcd = pcd0;

	mu_compile_entry();
	test_comp_fetch_lit(0x00007FFF);
	test_comp_fetch_lit(0xFFFF8000);
	test_comp_fetch_lit(0x00000000);
	test_comp_fetch_lit(0xFFFFFFFF);
	test_comp_fetch_lit(0xFFFF7FFF);
	test_comp_fetch_lit(0x00000001);
	test_comp_fetch_lit(0x12345678);
	test_comp_fetch_lit(0x87654321);
	test_comp_fetch_lit(0x00008000);
	test_comp_fetch_lit(0xFEDCBA98);
	PUSH(5);
	mu_compile_literal_load();
	mu_compile_literal_push();
	mu_compile_push_to_r();
	mu_compile_copy_from_r();
	mu_compile_pop_from_r();
	mu_compile_2drop();
	mu_compile_exit();

	PUSH(pcd0);
	mu_execute();

	printf("Done.\n");

	return 0;
}
#endif /* PPC_TEST */
