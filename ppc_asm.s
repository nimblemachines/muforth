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
.text				; section declaration - begin code

	.globl mu_iflush_addr
mu_iflush_addr:
	dcbst	0,r3		; update memory
	sync			; wait for update
	icbi	0,r3		; remove (invalidate) copy in instruction cache
	sync			; wait for ICBI operation to be globally performed
	isync			; remove copy in own instruction buffer
	blr			; Return

.data
				; This data segment is bogus, and used for gdb to not wig-out
