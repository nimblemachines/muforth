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
	stwu	r14,-4(r1)		; Preserve R14 before putting the address
					; of "sp" into it.
	stwu	r15,-4(r1)

	addi	r14,0,lo16(_sp)		; r14 = &sp
	addis	r14,r14,ha16(_sp)
	lwz	r15,0(r14)		; r15 =  sp
	
	lwz	r3,0(r15)		; Fetch the address of the next word to call
	addi	r15,r15,4		; Increment the Stack Pointer (in the register)
	
	mtlr	r3			; Prepare the target address
	blrl				; Branch to the function

	stw	r15,0(r14)		; Save SP before returnning to C

	lwz	r15,0(r1)		; Restore R15 for the caller (C code)
	lwz	r14,4(r1)		; Restore R14 for the caller (C code)
	lwz	r0,8(r1)		; Restore the Link Register
	mtlr	r0
	addi	r1,r1,12		; Drop R14 and the Link Register from the stack
	blr				; Return to the calller

.data
				; This data segment is bogus, and used for gdb to not wig-out
