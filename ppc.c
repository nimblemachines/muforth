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
static cell_t *pcd_last_call;

/************************************************************************
 *
 * PPC Instructions
 * 
 * I chose not to embed instruction opcodes directly into the C code
 * but instead defined them here.  Hopefully this helps to code to be
 * readable.
 */
#define INSTR_MASK	0xFC000000
#define INSTR_ENCODING	0x000007FF

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
#define BR_COND_ZERO	(BRCOND | RS(12) | RA(2))
#define BR_COND_NZERO	(BRCOND | RS(4)  | RA(2))

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
#define CLASS_31	0x7C000000
#define MTSPR_ENCODING	0x000003a6
#define MFSPR_ENCODING	0x000002a6
#define MTSPR		(CLASS_31 | MTSPR_ENCODING)
#define MFSPR		(CLASS_31 | MFSPR_ENCODING)
#define SPR_LR		(RB(0) | RA(8))
#define SPR_CTR		(RB(0) | RA(9))
#define MTLR		(MTSPR | SPR_LR)
#define MFLR		(MFSPR | SPR_LR)


#define SHIFT_LEFT	0x7C000030

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
 * Also, R11 contains a pointer to the global variable sp.
 * This is good for two reasons.  First, Forth words don't have
 * to generate code to load the address of the stack pointer
 * (2 instructions) and C code won't tromple on top of it.
 * The reason for this is that R11 is a nonvolatile register
 * and thus the compiler will generate code to preserve R11
 * before munging it and will reserect it before returning.
 * However, if a C routine called a Forth word and that
 * C routine had pushed R11 and then overwritten it, then
 * the Forth word would execute without the benefit of R11.
 * Hmmm.  Is it possible for C functions to execute Forth
 * routines?  How does this system get started?  What's
 * the transition between C and Forth?
 */

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
#define R_RP		1
#define R_TEMP		3
#define R_A		11
#define R_B		12
#define R_CALL		7
#define R_LINK_IN	8
#define R_LINK_OUT	9
#define R_PUSH_TEMP	10
#define R_PSP		4
#define R_SP		5
#define NUM_REGS	32


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
#define ADD_ENCODING	0x00000214
#define ADD		(CLASS_31 | ADD_ENCODING)
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
 * ppc_iflush_addr();
 *
 * This routine is defined in assembly language (see ppc_asm.s).
 * It will flush a cache block from the d-cache and i-cache.
 */
extern void ppc_iflush_addr(void *addr);
extern void ppc_setup_regs(void);
extern void ppc_get_tos(void);
extern void ppc_set_tos(void);
extern void ppc_pop(void);
extern void ppc_push(void);
extern void ppc_push_lit(void);
extern void ppc_over(void);
extern void ppc_swap(void);
extern void ppc_drop(void);
extern void ppc_2drop(void);
extern void ppc_pop_from_r(void);
extern void ppc_copy_from_r(void);
extern void ppc_push_to_r(void);
extern void ppc_next(void);
extern void ppc_exit(void);

/*
 * store_instr() and comma_instr()
 *
 * This routine will append the instruction into the instruction stream.
 * And, it flushes the instruction from the data stream.
 *
 * All instructions that modify memory filter through this routine.
 * This will change when I write code to modify already placed instructions.
 */
static void store_instr(cell_t *addr, int instr)
{
	*addr = instr;

	ppc_iflush_addr(addr);
}

static void comma_instr(int instr)
{
	cell_t *pc = (u_int32_t *) pcd;

	store_instr(pc, instr);

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

/*
 * comma_fetch_sp(), comma_store_sp()
 *
 * These routines "manage" the loading/storing of the stack pointer.
 * The logic is simple: A call to comma_fetch_sp() is used in any
 * routine that expects the stack pointer to be valid.
 * Then, when calling C code or exiting a function, comma_store_sp()
 * is called.  If the stack pointer register had been modified
 * then it will be written back to memory for others to use.
 *
 * NOTE: for now, no tracking of a modified stack pointer is kept.
 * Therefore, we always write sp back to main memory at exit points.
 */
typedef void (*ppc_func)(void);
static void comma_li(int r, int val);
static void comma_ld(int r_dest, unsigned int offset, int r_base);
static void comma_st(int r_src, int offset, int r_base);
static void comma_safe_call(ppc_func addr);

/*
 * comma_<ppc-instr>()
 *
 * A bunch of routines which build various PowerPC instructions.
 * I use these instead of in-line build instructions (with direct
 * calls to comma_instr() because various errors can be checked
 * here.  Also, it makes the call points look more readable.
 */

static void comma_safe_call(ppc_func func)
{
	cell_t addr = (cell_t) func;

	comma_instr(BR | addr | BR_ABS_ADDR | BR_AND_LINK);
}

static void comma_fetch_sp(void)
{
	comma_safe_call(ppc_setup_regs);
}

static void comma_store_sp(void)
{
	comma_st(R_SP, 0, R_PSP);
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

static void comma_ld(int r_dest, unsigned int offset, int r_base)
{
	assert_valid_reg(r_dest);
	assert_valid_reg(r_base);
	assert_16bit_val(offset);

	if (r_dest == 3 && r_base == R_SP) {
		comma_safe_call(ppc_get_tos);
	} else {
		if (r_base == R_SP)
			comma_fetch_sp();

		comma_instr(LD | RS(r_dest) | RA(r_base) | (offset & IMM_MASK));
	}
}

static void comma_st(int r_src, int offset, int r_base)
{
	assert_valid_reg(r_src);
	assert_valid_reg(r_base);
	assert_16bit_val(offset);

	if (r_src == 3 && r_base == R_SP && offset == 0) {
		comma_safe_call(ppc_set_tos);
	} else {
		if (r_base == R_SP)
			comma_fetch_sp();

		comma_instr(ST | RS(r_src) | RA(r_base) | (offset & IMM_MASK));
	}
}

static void comma_stu(int r_src, int offset, int r_base)
{
	assert_valid_reg(r_src);
	assert_valid_reg(r_base);
	assert_16bit_val(offset);

	if (r_base == R_SP)
		comma_fetch_sp();

	comma_instr(STU | RS(r_src) | RA(r_base) | (offset & IMM_MASK));
	
	if (r_base == R_SP)
		comma_store_sp();
}

static void comma_cmpi(int r, int value)
{
	assert_valid_reg(r);
	assert_16bit_val(value);

	if (r == R_SP)
		comma_fetch_sp();

	comma_instr(CMPI | RA(r) | value);
}

static void comma_addi(int reg, int val)
{
	assert_valid_reg(reg);

	if (reg == R_SP)
		comma_fetch_sp();

	comma_instr(ADDI | RS(reg) | RA(reg) | (val & IMM_MASK));

	if (reg == R_SP)
		comma_store_sp();
}

#if 0
static void comma_mtlr(int r)
{
	comma_instr(MTLR | RS(r));
}
#endif

static void comma_mflr(int r)
{
	comma_instr(MFLR | RS(r));
}

static void comma_pop(int r, int r_sp)
{
	if (r == 3 && r_sp == R_SP) {
		comma_safe_call(ppc_pop);
	} else {
		comma_ld(r, 0, r_sp);
		comma_addi(r_sp, 4);
	}
}

static void comma_push(int r, int r_sp)
{
	if (r == 3 && r_sp == R_SP) {
		comma_safe_call(ppc_push);
	} else {
		comma_stu(r, -4, r_sp);
	}
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
static int is_c_function(u_int32_t addr)
{
	if (addr < (u_int32_t) pcd0)
		return 1;
	else
		return 0;
}

static void prep_for_c_call(int addr)
{
	cell_t *pc;

	if (is_c_function(addr))
	{
		pc = (cell_t *) pcd;

		if (0 && pc[-1] == (ADDI | RS(R_RP) | RA(R_RP) | 16)) {
			/*
			 * Overwrite the tear-down instruction
			 * with the branch instruction
			 */
			pcd -= 4;
		} else {
			/*
			 * Build a call frame
			 */
			comma_addi(R_RP, -16);
		}
	}
}

static void done_with_c_call(int addr)
{
	if (is_c_function(addr))
		comma_addi(R_RP, 16);
}

static void comma_call(int addr)
{
	prep_for_c_call(addr);
	comma_instr(BR | addr | BR_ABS_ADDR | BR_AND_LINK);
	done_with_c_call(addr);
}

static void comma_jump(ppc_func func)
{
	cell_t addr = (cell_t) func;

	comma_instr(BR | addr | BR_ABS_ADDR);
}

void mu_compile_call()
{
	u_int32_t addr;

	pcd_last_call = (cell_t *) pcd;

	addr = POP;

	/*
	 * ERROR: We cannot handle calling a function at an address
	 * that is not 4 byte aligned.
	 */
	assert(0 == (addr & 3));

	if (addr == (cell_t) mu_over)
		comma_safe_call(ppc_over);
	else if(addr == (cell_t) mu_swap)
		comma_safe_call(ppc_swap);
	else
		comma_call(addr);
}

/* resolve a forward or backward jump - moved from startup.mu4 because it
 * was i386-specific. In this usage "src" points just _past_ the displacement
 * we need to fix up.
 * : resolve   ( src dest)  over -  swap cell- ! ( 32-bit displacement) ;
 */
void mu_resolve(void)
{
	u_int32_t *dest = (u_int32_t *) POP;
	u_int32_t *src  = (u_int32_t *) POP -1;
	u_int32_t instr, br_type, delta;

	delta = (u_int32_t) dest - (u_int32_t) src;
	instr = *src;
	br_type = instr & INSTR_MASK;
	if (br_type == BR) {
		delta &= ~INSTR_MASK;
	} else if (br_type == BRCOND) {
		br_type |= instr & (RS(31) | RA(31));
		delta &= ~(INSTR_MASK | RS(31) | RA(31));
	} else {
		assert(0);
	}
	assert((delta & 3) == 0);

	store_instr(src, br_type | delta);
}

void mu_compile_literal_load()
{
	comma_li(R_PUSH_TEMP, POP);
}

void mu_compile_literal_push()
{
	comma_safe_call(ppc_push_lit);
}

void mu_compile_drop(void)
{
	comma_safe_call(ppc_drop);
}

void mu_compile_2drop(void)
{
	comma_safe_call(ppc_2drop);
}

void mu_compile_entry(void)
{
	comma_mflr(R_LINK_IN);
	comma_push(R_LINK_IN, R_RP);
}

/*
 * mu_compile_return()
 *
 * This routine would like to turn some calls into tail recursive calls.
 * In order to do so, at the point where we would place the return code
 * sequence the following things must be true:
 * 1) if the previous instruction is a branch and link absolute address,
 * 2) Or, if the previous instruction is a branch to link address and link.
 *
 * If either is true, then we can -in theory- convert the calls into jumps.
 * But, in order to do that one other condition must be true:
 *    The call must be to a Forth compiled function.
 * The reason for this last condition is that because of the link register
 * mechanism of function calls, we have to skip the pushing of the link
 * register onto the stack.  The only way to do that is to know how long
 * that code sequence is (or de-assemble the code on the fly to determine it!).
 * With Forth code it's relatively easy to know: It's two instructions.
 * So, we will need to construct a new kind of instruction with the new
 * address and ensure that it fits within the space allotted for the call
 * already.  So long as that's all fine, then we can replace a call
 * with a jump.
 *
 * It's much easier on architectures with JSR instructions.
 */
void mu_compile_exit(void)
{
#if 0
	u_int32_t *pc;

	/*
	 * Check to see if the previous instruction is a BR_ABS_ADDR
	 * or a BLRL_ALWAYS, then disable their AND_LINK bits.
	 */
	pc = (u_int32_t *) pcd -1;
	if ((*pc & (BAL | BR_ABS_ADDR)) == (BAL | BR_ABS_ADDR)) {
		*pc &= ~BR_AND_LINK;
	} else if (*pc == BLRL_ALWAYS) {
		*pc &= ~BR_AND_LINK;
	}
#endif

	comma_jump(ppc_exit);

#if 0
	comma_pop(R_LINK_OUT, R_RP);
	comma_mtlr(R_LINK_OUT);
	comma_instr(BLR_ALWAYS);
#endif
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
	comma_safe_call(ppc_pop_from_r);
}

void mu_compile_2pop_from_r()
{
	/*
	 * This could be optimized, later.
	 */
	comma_safe_call(ppc_pop_from_r);
	comma_safe_call(ppc_pop_from_r);
}

void mu_compile_copy_from_r()
{
	comma_safe_call(ppc_copy_from_r);
}

void mu_compile_push_to_r()
{
	comma_safe_call(ppc_push_to_r);
}

void mu_compile_2push_to_r()
{
	/*
	 * This could be optimized, later.
	 */
	comma_safe_call(ppc_push_to_r);
	comma_safe_call(ppc_push_to_r);
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

	assert(0);

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
 * code sequence.  Or, the top of stack points to the beginning of
 * a word.  Therefore, the first instructions will be the beginning
 * of a word, i.e.: function prolog instructions.  We need to skip
 * the function prolog.
 *
 * The first instruction (after handling the prolog) will be an ADDI or an
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

	/*
	 * The following sequence needs to be coordinated with the
	 * function mu_compile_entry();
	 */
	if (instr == (MFLR | RS(R_LINK_IN)))
		instr = *++addr;

	if (instr == (STU | RS(R_LINK_IN) | RA(R_RP) | (-4 & IMM_MASK)))
		instr = *++addr;

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
	comma_safe_call(ppc_next);	/* R_TEMP = (*R_RP) ++; */
	comma_cmpi(R_TEMP, 0);		/* z = (R_TEMP == 0); */
	comma_instr(BR_COND_NZERO);	/* } while (!z); */
	PUSH(pcd);			/* Compiler push of the branch address */
	comma_addi(R_RP, 4);		/* pop the count off the return stack */
}

/*************************************************************
 *
 * ppc_disassembler()
 *
 * Here's a simple word disassembler.
 *
 *************************************************************
 */

extern cell_t print_func_name(cell_t addr);

static cell_t ppc_test_func(cell_t addr, ppc_func func, char *func_name)
{
	if (addr == (cell_t) func) {
		printf("%p: %s", func, func_name);
		return addr;
	} else {
		return 0;
	}
}

#define PPC_TEST_FUNC(func_name)	if (!base) base = ppc_test_func(addr, func_name, #func_name)

static cell_t ppc_print_func_name(cell_t addr)
{
	cell_t base;

	base = 0;
	PPC_TEST_FUNC(ppc_setup_regs);
	PPC_TEST_FUNC(ppc_get_tos);
	PPC_TEST_FUNC(ppc_set_tos);
	PPC_TEST_FUNC(ppc_pop);
	PPC_TEST_FUNC(ppc_push);
	PPC_TEST_FUNC(ppc_push_lit);
	PPC_TEST_FUNC(ppc_over);
	PPC_TEST_FUNC(ppc_swap);
	PPC_TEST_FUNC(ppc_drop);
	PPC_TEST_FUNC(ppc_2drop);
	PPC_TEST_FUNC(ppc_pop_from_r);
	PPC_TEST_FUNC(ppc_copy_from_r);
	PPC_TEST_FUNC(ppc_push_to_r);
	PPC_TEST_FUNC(ppc_next);
	PPC_TEST_FUNC(ppc_exit);

	if (!base)
		base = print_func_name(addr);

	return base;
}

void ppc_disassemble(int addr)
{
	cell_t *pc, base, instr, type;
	int done, rs, ra, rb, imm, encoding;
	int mflr;

	mflr = MFLR | RS(R_LINK_IN);

	base = ppc_print_func_name(addr);
	printf("\n");

	pc = (cell_t *) base;
	while (base <= addr) {
		if (*(cell_t *)base == mflr)
			pc = (cell_t *) base;
		base += 4;
	}
	base = (cell_t) pc;

	done = 0;
	do {
		instr = *pc;

		type = instr & INSTR_MASK;
		rs = (instr & RS(31)) >> RS_SHIFT;
		ra = (instr & RA(31)) >> RA_SHIFT;
		rb = (instr & RB(31)) >> RB_SHIFT;
		encoding = (instr & INSTR_ENCODING);
		imm = instr & IMM_MASK;
		if (imm & 0x8000) {
			imm |= 0xFFFF0000;
		}
		
		if (instr == mflr && (cell_t) pc != base)
			break;

		printf("%p: ", pc);

		switch(type) {
		case CLASS_31:
			switch (encoding) {
			case ADD_ENCODING:
				printf("add\tr%d, r%d, r%d\n", rs, ra, rb);
				break;
			case MTSPR_ENCODING:
				printf("mtlr\tr%d\n", rs);
				break;
			case MFSPR_ENCODING:
				printf("mflr\tr%d\n", rs);
				break;
			default:
				assert(0);
			}
			break;

		case CMPI:
			printf("cmpi\tr%d, %d\n", ra, imm);
			break;

		case ADDI:
			if (ra == 0) {
				cell_t instr2, type2, rs2, ra2, imm2, val;
				instr2 = pc[1];
				type2 = instr2 & INSTR_MASK;
				rs2 = (instr2 & RS(31)) >> RS_SHIFT;
				ra2 = (instr2 & RA(31)) >> RA_SHIFT;
				imm2 = (instr2 & IMM_MASK) << 16;
				val = imm + imm2;
				if (type2 == ADDIS && ra2 == rs) {
					if (val == (cell_t) &sp)
						printf("li\tr%d, &sp\n", rs);
					else
						printf("li\tr%d, %d\n", rs, val);
					pc ++;
				} else {
					printf("li\tr%d, %d\n", rs, imm);
				}
			} else {
				printf("addi\tr%d, r%d, %d\n", rs, ra, imm);
			}
			break;

		case ADDIS:
			printf("addis\tr%d, r%d, %d\n", rs, ra, imm);
			break;

		case STU:
			printf("stwu\tr%d, %d(r%d)\n", rs, imm, ra);
			break;

		case ST:
			printf("st\tr%d, %d(r%d)\n", rs, imm, ra);
			break;

		case LD:
			printf("ld\tr%d, %d(r%d)\n", rs, imm, ra);
			break;

		case BRCOND:
			if (rs == 12)
				printf("beq\t");
			else if (rs == 4)
				printf("bne\t");
			else
				assert (0);
			printf("%#x\n", (cell_t) pc + imm);
			break;

		case BR:
		{
			cell_t dest;

			dest = instr & ~(INSTR_MASK | BR_ABS_ADDR | BR_AND_LINK);
			if (dest & 0x02000000)
				dest |= 0xFC000000;
			if (instr & BR_AND_LINK)
				printf("bal\t");
			else
				printf("ba\t");
			if (instr & BR_ABS_ADDR)
				ppc_print_func_name(dest);
			else
				printf("%#x", (cell_t) pc + dest);
			printf("\n");
			break;
		}

		case BLR_ALWAYS & INSTR_MASK:
			if (instr == BLRL_ALWAYS)
				printf("blrl\n");
			else
				printf("blr\n");
			done = 0;
			break;

		default:
			printf("UNKNOWN instr %8.8x\n", instr);
			done = 1;
		}

		pc ++;
	} while (!done);
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
