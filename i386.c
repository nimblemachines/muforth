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

/* Compiler essentials for Intel architecture */

#include "muforth.h"

static uint8_t *pcd_last_call;
static uint8_t *pcd_jump_dest = 0;

static void mu_compile_offset()
{
    uint8_t *dest = (uint8_t *) POP;

    *(uint32_t *)pcd = dest - (pcd + 4);
    pcd += 4;
}

void mu_compile_call()
{
    pcd_last_call = pcd;
    *pcd++ = 0xe8;		/* call near, 32 bit offset */
    mu_compile_offset();
}

#if 0
void mu_compile_jump()
{
    pcd_last_call = pcd;
    *pcd++ = 0xe9;		/* jump near, 32 bit offset */
    mu_compile_offset();
}
#endif

/*
 * mu_compile_entry()
 *
 * On some architectures some entry code is necessary.
 * On i386 this is not the case.
 */
void mu_compile_entry()
{
    /*
     * Nothing.
     */
}

void mu_compile_exit()
{
    /* convert call in tail position to jump */
    if (pcd - 5 == pcd_last_call && *pcd_last_call == 0xe8)
    {
	*pcd_last_call = 0xe9;	/* tail call --> jump */

	/* if this is also a jump destination, compile a return instruction */
	if (pcd == pcd_jump_dest)
	    *pcd++ = 0xc3;		/* ret near, don't pop any args */
    }
    else
	*pcd++ = 0xc3;		/* ret near, don't pop any args */
}

static void compile_stack_adjust(int n)
{
    uint32_t *help;

    help = (uint32_t *) pcd;
    help[0] = 0x0583;		/* addl $4,sp */
    help = (uint32_t *)(pcd + 2);
    help[0] = (uint32_t) &sp;
    help[1] = n & 0xff;		/* 8 bit offset */
    pcd += 7;
}
    
void mu_compile_drop()
{
    compile_stack_adjust(4);
}

void mu_compile_2drop()
{
    compile_stack_adjust(8);
}

void mu_compile_literal_load()
{
    uint32_t *help;

    *pcd = 0xba;		/* mov immed to edx */
    help = (uint32_t *)(pcd + 1);
    help[0] = POP;		/* value */
    pcd += 5;
}

void mu_fetch_literal_value()
{
    uint32_t *p;

    p = (uint32_t *)(TOP + 1);	/* TOP points to "movl #x, %edx" */
    TOP = *p;			/* fetch the value that is loaded */
}

void mu_compile_literal_push()
{
    PUSH((uint32_t) mu_push_literal);
    mu_compile_call();		/* sets pcd_last_call */
}

#ifdef NOTYET
void mu_was_literal()
{
    if (pcd - 5 == pcd_last_call && *pcd_last_call == 0xe8
	&& (pcd + *(uint32_t *)(pcd - 4) == (char *) &push_literal))
    {
	/* last code compiled was a call to push_literal; back up,
           uncompile the call, and push the literal value onto the stack */
	/* XXX */
    }
}
#endif

static void mu_compile_sp_to_eax()
{
    uint32_t *help;

    *pcd = 0xa1;		/* movl sp,%eax */
    help = (uint32_t *)(pcd + 1);
    help[0] = (uint32_t) &sp;
    pcd += 5;
}

/* This requires that eax be loaded with sp, either with (for if) or
   without (for =if) a stack adjustment. */

/* After running out of "displacement space" in a Forth word, I've changed
   the branch compilers to use the 32-bit displacement versions. */

/* XXX: It might make sense to make _forward_ branches always 32 bits -
   since we don't know ahead of time how far we'll have to jump - but
   make backwards branches only as long as they need to be. */

static void compile_zbranch()
{
    uint32_t *help;

    help = (uint32_t *) pcd;
    help[0] = 0x0f003883; 	/* cmpl $0, (%eax); je rel32 */
    help[1] = 0x00000084;
    pcd += 9;
    PUSH(pcd);			/* 32 bit offset to fixup at (top)-1 */
}

void mu_compile_destructive_zbranch()
{
    mu_compile_sp_to_eax();
    mu_compile_drop();
    compile_zbranch();
}

void mu_compile_nondestructive_zbranch()
{
    mu_compile_sp_to_eax();
    compile_zbranch();
}


void mu_compile_branch()
{
    *pcd = 0xe9;		/* jmp rel32 unconditionally */
    pcd += 5;
    PUSH(pcd);			/* offset to fix up later */
}

/* resolve a forward or backward jump - moved from startup.mu4 because it
 * was i386-specific. In this usage "src" points just _past_ the displacement
 * we need to fix up.
 * : resolve   ( src dest)  over -  swap cell- ! ( 32-bit displacement) ;
 */
void mu_resolve()
{
    uint32_t src = STK(1);
    uint32_t dest = TOP;
    uint32_t *psrc = (uint32_t *)src;
    psrc[-1] = dest - src;
    DROP(2);

    /* also set up last jump destination, for tail-call code */
    pcd_jump_dest = (uint8_t *)dest;
}


/* We don't need 2shunt because we can call this as many times as
 * necessary; it only compiles 1 byte per call!
 */
void mu_compile_shunt()		/* drop 4 bytes from r stack */
{
    *pcd++ = 0x58;		/* popl %eax */
}


static void compile_stack_move(int n)
{
    mu_compile_sp_to_eax();
    compile_stack_adjust(n);
}
    
/* Next challenge: for/next. These require popping and pushing the _other_
 * stack: the return (hardware) stack.
 */

void mu_compile_pop_from_r()
{
    compile_stack_move(-4);	/* eax = sp; sp-- */
    *(uint32_t *) pcd = 0xfc408f;	/* popl -4(%eax) */
    pcd += 3;
    
}

void mu_compile_2pop_from_r()
{
    uint32_t *help;

    compile_stack_move(-8);	/* eax = sp; sp -= 2 */
    help = (uint32_t *) pcd;
    help[0] = 0x8ff8408f;	/* popl -8(%eax); popl -4(%eax) */
    help[1] = 0xfc40;
    pcd += 6;
}

/*
push = ff /6 = ff 060
pop  = 8f /0 = 8f 000
0(reg) = 00r
8b(reg) = 10r
32b(reg) = 20r
reg      = 30r
eax = 0

add this up: pushl (eax) = ff 060 + 000 = ff 30
popl -4(eax) = 8f 000 + 100 fc = 8f 40 fc

mov r/m to reg is 8b /r
mov reg to r/m is 89 /r
edx = 2
0(esp) = 0r4 sib = 0r4 044 = 14 24
sib = 04r for no index reg (using 4 = esp's index for all the special cases)
*/

void mu_compile_push_to_r()
{
    compile_stack_move(4);	/* eax = sp; sp++ */
    *(uint32_t *) pcd = 0x30ff;	/* pushl (%eax) */
    pcd += 2;
}

void mu_compile_2push_to_r()
{
    uint32_t *help;

    compile_stack_move(8);	/* eax = sp; sp += 2 */
    help = (uint32_t *) pcd;
    help[0] = 0xff0470ff;	/* pushl 4(%eax); pushl (%eax) */
    help[1] = 0x30;
    pcd += 5;
}

void mu_compile_copy_from_r()
{
    uint32_t *help;

    compile_stack_move(-4);	/* eax = sp; sp-- */
    help = (uint32_t *) pcd;
    help[0] = 0x8924148b;	/* movl (%esp),%edx; movl %edx,-4(%eax) */
    help[1] = 0xfc50;
    pcd += 6;
}

void mu_compile_qfor()
{
    compile_stack_move(4);	/* eax = sp; sp++ */
    compile_zbranch();		/* cmpl $0, (%eax); je rel32 */
    *(uint32_t *) pcd = 0x30ff;	/* pushl (%eax) */
    pcd += 2;
    PUSH(pcd);			/* push address to loop back to */
}

void mu_compile_next()
{
    uint32_t *help;

    help = (uint32_t *) pcd;
    help[0] = 0x0f240cff;	/* decl (%esp); jne <back32> */
    help[1] = 0x00000085;
    help[2] = 0x5800;		/* popl %eax */
    pcd += 10;
    PUSH(pcd - 1);		/* for fixup of jne */
}
