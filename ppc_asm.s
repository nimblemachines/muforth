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

	.globl	_mu_stack_adjust
	.globl _sp
_mu_stack_adjust:
	oris	r11,r0,ha16(_sp)
	addi	r11,r11,lo16(_sp)
	lwz	r12,0(r11)		; Get the stack pointer
	sub	r12,r12,r3		; Subtract r3 from r11 to adjust the stack.
	stw	r12,0(r11)
	blr

	.globl	_mu_push_literal
_mu_push_literal:
	oris	r11,r0,ha16(_sp)
	addi	r11,r11,lo16(_sp)
	lwz	r12,0(r11)		; Get the stack pointer
	subi	r12,r12,4
	stw	r3,0(r12)		; Store the passed parameter (r3) on the stack
					; R1 -4
	stw	r12,0(r11)
	blr
	
.data
				; This data segment is bogus, and used for gdb to not wig-out
