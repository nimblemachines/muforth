##  $Id$
##
##  This file is part of muforth.
##
##  Copyright 1997-2005 David Frech. All rights reserved, and all wrongs
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

## After struggling with gcc for a long time, I gave up and wrote these
## routines in assembler.

    .globl  mu_dplus
mu_dplus:
    movl    SP,%eax
	movl    0(%eax),%edx  # SND = lo1
    addl    %edx,8(%eax)  # FTH = lo2
	movl    T,%edx
    adcl    4(%eax),%edx  # sum hi = T + TRD
    movl    %edx,T
    addl    $8,SP
    ret

    .globl  mu_dnegate
mu_dnegate:
    movl    SP,%eax
    negl    T
    negl    0(%eax)
    sbbl    $0,T
    ret

    .globl  mu_um_star
mu_um_star:
    movl    SP,%ecx
    movl    T,%eax
    mull    0(%ecx)
    movl    %edx,T        # product hi
    movl    %eax,0(%ecx)  # product lo
    ret

    .globl  mu_m_star
mu_m_star:
    movl    SP,%ecx
    movl    T,%eax
    imull   0(%ecx)
    movl    %edx,T        # product hi
    movl    %eax,0(%ecx)  # product lo
    ret

    .globl  mu_um_slash_mod
mu_um_slash_mod:
    movl    SP,%ecx
    movl    0(%ecx),%edx    # dividend hi
    movl    4(%ecx),%eax    # dividend lo
    divl    T               # divisor on stack
    movl    %edx,4(%ecx)    # remainder
    movl    %eax,T          # quotient, on top
    addl    $4,SP
    ret

    .globl  mu_sm_slash_rem     # symmetric division, for comparison
mu_sm_slash_rem:
    movl    SP,%ecx
    movl    0(%ecx),%edx    # hi word at lower address
    movl    4(%ecx),%eax    # dividend = edx:eax, divisor on stack
    idivl   T               # now rem = edx, quotient = eax
    movl    %edx,4(%ecx)    # leave remainder and
    movl    %eax,T          # quotient on stack, quotient on top
    addl    $4,SP
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

    .globl  mu_fm_slash_mod     # floored division!
mu_fm_slash_mod:
    pushl   %ebx            # callee saved!
    movl    SP,%ecx
	movl    T,%ebx
    movl    0(%ecx),%edx    # hi word at lower address
    movl    4(%ecx),%eax    # dividend = edx:eax, divisor on stack
    idivl   %ebx            # now modulus = edx, quotient = eax  
    xorl    0(%ecx),%ebx    ## <0 if d'dend & d'sor are opposite signs
    jns     1f              # do nothing if same sign
    orl     %edx,%edx
    je      1f              # do nothing if mod == 0
    decl    %eax            # otherwise quot = quot - 1
    addl    T,%edx          #            mod = mod + divisor
1:  movl    %edx,4(%ecx)    # leave modulus and
    movl    %eax,T          # quotient on stack, quotient on top
    addl    $4,SP
    popl    %ebx            # callee saved!
    ret
