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
 * BLR_ALWAYS is Branch to Link Register, Always
 */
#define BLR_ALWAYS	0x4e800020

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
 *
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
 * Also, it tracks region a region that needs flushing.  If the region
 * continues to be continuous, then it simply notes that.  If the region
 * breaks, it will issue a flush.
 */
static u_int32_t *flush_base;
static   int32_t  flush_len = -1;

static void mu_flush_code_stream(void *base, int32_t len)
{
	char *p;

	p = (char *) base;

	while (len > 0) {
		mu_iflush_addr(p);
		p += CACHELINE_SIZE;
		len -= CACHELINE_SIZE;
	}
}

static void mu_increase_flush_len(u_int32_t *base, int32_t len)
{
	if (flush_len >= 0) {
		if (flush_base == base) {
			flush_len += len;
			return;
		} else {
			/*
			 * Else flush and begin a new region
			 */
			mu_flush_code_stream(flush_base, flush_len);
		}
	}

	flush_len = len;
	flush_base = base;
}

static void mu_iflush(void)
{
	mu_flush_code_stream(flush_base, flush_len);
}

static void mu_comma_instr(int instr)
{
	u_int32_t *pc = (u_int32_t *) pcd;

	*pc = instr;

	mu_increase_flush_len(pc, 4);

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

	if (addr & 0xFC000000) {
		u_int32_t lo, hi;

		/*
		 * The address must be loaded into a temporary register
		 * and we can jump to it.
		 */
		lo = addr >>  0 & 0xFFFF;
		hi = addr >> 16 & 0xFFFF;

		if (lo & 0x8000)
			hi += 1;

		mu_comma_instr(ADDI  | RB_TEMP1 | lo);
		mu_comma_instr(ADDIS | RB_TEMP1 | hi);
		mu_comma_instr(MTLR(R_TEMP1));
		mu_comma_instr(BLR_ALWAYS);
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

