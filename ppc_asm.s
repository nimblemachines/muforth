	;; 
	;; $Id$
	;; 
	;; This file is part of muforth.
	;; 
	;; Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
	;; reversed.
	;; 
	;; Licensed under the Apache License, Version 2.0 (the "License");
	;; you may not use this file except in compliance with the License.
	;; You may obtain a copy of the License at
	;; 
	;;     http://www.apache.org/licenses/LICENSE-2.0
	;; 
	;; or see the file LICENSE in the top directory of this distribution.
	;; 
	;; Unless required by applicable law or agreed to in writing, software
	;; distributed under the License is distributed on an "AS IS" BASIS,
	;; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	;; See the License for the specific language governing permissions and
	;; limitations under the License.
	;;
	;; The calling convention for all Forth function is that all parameters
	;; are on the data stack (pointed to by the global variable _sp).
	;; There are no parameters passed on the PPC's stack (register 1).
	;; 
	;; ppc_asm.s
	;;
	;; This file exports functions which contain instructions
	;; not available to C programmers.
	;;
	;; mu_iflush_addr(u_int32_t *addr)
	;;	Given an address, this routine will flush the cache line containing
	;;	that address in the d-cache and i-cache.  These are the instructions
	;;	as specified by IBM in the PPC 750 programmer's guide.
	;;
	;; mu_stack_adjust(int n)
	;;      This routine will add N to the stack pointer, R1.
	;; 
.text					; section declaration - begin code

	.globl	_mu_iflush_addr
_mu_iflush_addr:
	dcbst	0,r3			; update memory
	sync				; wait for update
	icbi	0,r3			; remove (invalidate) copy in instruction cache
	sync				; wait for ICBI operation to be globally performed
	isync				; remove copy in own instruction buffer
	blr				; Return


	;;
	;; mu_execute
	;;
	;; This is the function that gets called from C to invoke a Forth
	;; word.  This routine sets up PowerPC registers to hold special
	;; values.  See below for the exact list.  Some registers are pointers
	;; and some are values.  For example, suppose R14 holds the pointer
	;; to the C variable "sp" and R15 holds its value.  When Forth routines
	;; call back into C, the C routines will (if they were to use those
	;; registers) preserve them (because they are nonvolatile).  For the
	;; pointer register, there's no problem because the pointers will be
	;; the same before/after (the C variable won't move to a new address).
	;; However, the registers which hold a value may be invalid.  So,
	;; the routines which compile function calls will have to check to see
	;; if the routines called are C routines or Forth routines.  For Forth
	;; routines the values are guaranteed to be correct, but for C code
	;; that may not be the case.  Therefore, when C code is called from
	;; Forth, the value registers must be reloaded so they will be correct.
	;;
	;; Special Registers Used in this Forth
	;; r14:	Pointer to Data Stack Pointer
	;; r15:	Data Stack Pointer (note: requires reload after calling C code)
	;; 
	.globl	_mu_execute
	.globl _sp
_mu_execute:
	mflr	r0
	stwu	r0,-4(r1)		; Save return address

	addi	r11,0,lo16(_sp)		; r11 = &sp
	addis	r11,r11,ha16(_sp)
	lwz	r12,0(r11)		; r12 =  sp
	
	lwz	r3,0(r12)		; Fetch the address of the next word to call
	addi	r12,r12,4		; Increment the Stack Pointer (in the register)
	stw	r12,0(r11)		; Save new Stack Pointer (to memory)
	
	mtlr	r3			; Prepare the target address
	blrl				; Branch to the function

	lwz	r0,0(r1)		; Restore the Link Register
	mtlr	r0
	addi	r1,r1,4			; Drop R11 and the Link Register from the stack
	blr				; Return to the calller

	;;
	;; mu_jump
	;;
	;; This routine is the glue for jumping to an indexed
	;; Forth word following the current word, JUMP.
	;; The basic idea is to get the return address (which points
	;; to the 0th indexed word).  We then call into C code to
	;; do all the heavy thinking and then it returns here.
	;; After that we jump to the word at that index.  Note that
	;; by jumping to that word, the return from that word
	;; actually is -our- return.  Thefore, we only execute
	;; one in the vector list of words following jump and then
	;; exit from this word.
	.globl	_mu_jump
	.globl	_mu_jump_helper
_mu_jump:
	mflr	r3			; R3 is the first passed parameter in C on PPC
	addi	r11,0,lo16(_sp)		; r11 = &sp
	addis	r11,r11,ha16(_sp)
	stw	r12,0(r11)		; Save the stack pointer
	bl	_mu_jump_helper		; Call the C routine
	mtlr	r3			; R3 is the return value form the C routine and tells us where to go
	lwz	r15,0(r14)		; Restore the Stack Pointer from memory
	blr				; Jump to that word.

	;;
	;; mu_dnegate(), mu_dplus()
	;;
	;; These routines are written in assembler for no particularly good
	;; reason.
	;; 
	;; I will reschedule these instructions to reduce
	;; pipe-line bubbles after everything else works.
	.globl	_mu_dnegate
_mu_dnegate:
	addis	r11,0,ha16(_sp)		; r11 = &sp
	lwz	r12,lo16(_sp)(r11)	; r12 = sp
	lwz	r2,4(r12)		;  r2 = lo
	lwz	r3,0(r12)		;  r3 = hi
	subfic	r2,r2,0
	subfze	r3,r3
	stw	r2,4(r12)
	stw	r3,0(r12)
	blr

	;; The comments are written from the point of view
	;; of two d-cells a & b on the stack to be summed.
	;; 'a' is the name of the first item pushed.  As in:
	;;		a b d+
	;; Where each a and b are d-cells.
	;; I will reschedule these instructions to reduce
	;; pipe-line bubbles after everything else works.
	.globl	_mu_dplus
_mu_dplus:
	addi	r11,0,lo16(_sp)		; r11 = &sp
	addis	r11,r11,ha16(_sp)	; 
	lwz	r12,0(r11)		; r12 = sp
	lwz	r2,4(r12)		; r2 = b.lo
	lwz	r3,0(r12)		; r3 = b.hi
	lwz	r4,12(r12)		; r4 = a.lo
	lwz	r5,8(r12)		; r5 = a.hi
	addc	r2,r2,r4		; b.lo = b.lo + a.lo
	adde	r3,r3,r5		; b.hi = b.hi + a.hi + carry
	stw	r2,12(r12)
	stw	r3,8(r12)
	addi	r12,r12,8		; Pop the top d-cell
	stw	r12,0(r11)
	blr				; Return

.data
				; This data segment is bogus, and used for gdb to not wig-out
