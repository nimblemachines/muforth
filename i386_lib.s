##  $Id$
##
##  This file is part of muforth.
##
##  Copyright 1997-2004 David Frech. All rights reserved, and all wrongs
##  reversed.
##
##  Licensed under the Apache License, Version 2.0 [the "License"];
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##     http://www.apache.org/licenses/LICENSE-2.0
##
##  or see the file LICENSE in the top directory of this distribution.
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.)

	
## Note: mu_push_literal isn't really a function. DO NOT call it directly
## from "normal" Forth or C code! It is intended to be called by compiled
## code that is trying to do part of does> or part of a literal push. On
## entry edx contains the value we want to push.
	
	.globl	mu_push_literal
mu_push_literal:
	movl	sp,%eax
	sub	$4,sp
	movl	%edx,-4(%eax)	# value to push is in edx
	ret

## After struggling with gcc to create this code for me, I just wrote them
## by hand. Sigh.

	.globl	mu_dplus
mu_dplus:
	movl	sp,%eax
	movl	4(%eax),%edx
	addl	%edx,12(%eax)
	movl	0(%eax),%edx
	adcl	%edx,8(%eax)
	addl	$8,sp
	ret

	.globl	mu_dnegate
mu_dnegate:
	movl	sp,%eax
	negl	(%eax)
	negl	4(%eax)
	sbbl	$0,(%eax)
	ret

	.globl	mu_um_star
mu_um_star:
	movl	sp,%ecx
	movl	(%ecx),%eax
	mull	4(%ecx)
	movl	%edx,(%ecx)
	movl	%eax,4(%ecx)
	ret

	.globl	mu_m_star
mu_m_star:
	movl	sp,%ecx
	movl	(%ecx),%eax
	imull	4(%ecx)
	movl	%edx,(%ecx)
	movl	%eax,4(%ecx)
	ret

	.globl	mu_um_slash_mod
mu_um_slash_mod:
	movl	sp,%ecx
	movl	4(%ecx),%edx	# dividend hi
	movl	8(%ecx),%eax	# dividend lo
	divl	(%ecx)		# divisor
	movl	%edx,8(%ecx)	# remainder
	movl	%eax,4(%ecx)	# quotient
	addl	$4,sp
	ret

##  IDIV is symmetric.  To fix it (to make it _FLOOR_) we have to
##  adjust the quotient and remainder when BOTH rem /= 0 AND
##  the divisor and dividend are different signs.  (This is NOT the
##  same as quot < 0, because the quot could be truncated to zero
##  by symmetric division when the actual quotient is < 0!)
##  The adjustment is:
##    q' = q - 1
##    r' = r + divisor
##
##  This preserves the invariant a / b => (r,q) s.t. qb + r = a.
##
##  q'b + r' = (q - 1)b + (r + b) = qb - b + r + b
##           = qb + r
##           = a
##
##  where q',r' are the _floored_ quotient and remainder (really, modulus),
##  and q,r are the symmetric quotient and remainder.
##
##  XXX: discuss how the range of the rem/mod changes as the num changes
##  (in symm. div.) and as the denom changes (in floored div).

	.globl	mu_fm_slash_mod		# floored division!
mu_fm_slash_mod:
	pushl	%ebx		# callee saved!
	movl	sp,%ecx
	movl	4(%ecx),%edx
	movl	8(%ecx),%eax	# dividend
	idivl	(%ecx)		# divisor
	movl	(%ecx),%ebx
	xorl	4(%ecx),%ebx	## <0 if d'dend & d'sor are opposite signs
	jns	1f
	orl	%edx,%edx
	je	1f
	decl	%eax
	addl	(%ecx),%edx
1:	movl	%edx,8(%ecx)
	movl	%eax,4(%ecx)
	addl	$4,sp
	popl	%ebx		# callee saved!
	ret

## for jumping thru a following "vector" of compiled calls
	.globl	mu_jump
mu_jump:
	movl	sp,%eax
	addl	$4,sp

	# edx = vector = return address; eax = index
	popl	%edx
	movl	(%eax),%eax

	# edx points just _past_ the call/jmp instruction
	# we're interested in
	leal	5(%eax,%eax,4),%eax
	addl	%eax,%edx

	# ... so back up and get the offset, and add it to edx
	addl	-4(%edx),%edx
	jmp	*%edx		# then go there!

