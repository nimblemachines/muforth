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

/* compiler essentials for Power PC architecture */

#include "muforth.h"

/*
 * The PowerPC is a 32 bit processor with 32 bit instructions.
 * This file works completely with 32 bit entities.
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
#define	BAL		0x48000001
#define BAL_ABS_ADDR	0x00000002

/*
 * BLR_ALWAYS is Branch And Link to Link Register, Always
 *
 * After using the Link Register as the destination of the branch, the
 * address of the instruction following the branch is placed into the
 * Link Register.
 */
#define BCLRL_ALWAYS	0x4e800021

/*
 * Move To Link Register
 * This is actualy, move to special purpose register, lr.  Yeah, yeah.
 * When you use this you MUST specify a register to use, too.
 */
#define MTLR(Rn)	(0x7c0803a6 | ((Rn) << RS_SHIFT))

/*
 * A temporary register
 */
#define R_TEMP1		3

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

#define RB_TEMP1	(R_TEMP1 << RB_SHIFT)

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
#define ADDI		0x38000000
#define ADDIS		0x3C000000

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
 * mu_comma_instr(int instr)
 *
 * This routine will append the instruction into the instruction stream.
 * And, it flushes the instruction from the data stream.
 */
static void mu_comma_instr(int instr)
{
	u_int32_t *pc = (u_int32_t *) pcd;

	*pc = instr;

	mu_iflush_addr(pc);

	pcd += 4;
}

/*
 * mu_compile_call()
 *
 * The address of the entity to call is the top element of the stack.
 * This routine will place into the dictionary heap a call instruction
 * to that address.
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

	if ((addr == 0) || (addr & 0xFC000000)) {
		u_int32_t lo, hi;

		/*
		 * The address does not fit into a branch instruction.
		 * The address must be loaded into a temporary register
		 * so we can jump through it.
		 */
		lo = addr >>  0 & 0xFFFF;
		hi = addr >> 16 & 0xFFFF;

		if (lo & 0x8000)
			hi += 1;

		mu_comma_instr(ADDI  | RB_TEMP1 | lo);
		mu_comma_instr(ADDIS | RB_TEMP1 | hi);
		mu_comma_instr(MTLR(R_TEMP1));
		mu_comma_instr(BCLRL_ALWAYS);
	} else {
		/*
		 * The address is OK to jump to directly
		 */
		instr = BAL | addr | BAL_ABS_ADDR;

		/*
		 * Place the instruction in memory
		 */
		mu_comma_instr(instr);
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

void mu_compile_literal_push()
{
    extern void mu_push_literal(void);
    PUSH((int) mu_push_literal);
    mu_compile_call();		/* sets pcd_last_call */
}

void mu_compile_drop(void)
{
    compile_stack_adjust(4);
}

void mu_compile_2drop(void)
{
    compile_stack_adjust(8);
}

void mu_compile_return(void)
{
	
}

