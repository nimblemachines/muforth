/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2002 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* compiler essentials for intel architecture */

#include "muforth.h"

static u_int8_t *pcd_last_call;

static void compile_offset()
{
    u_int8_t *dest = (char *) POP;

    *(int *)pcd = dest - (pcd + 4);
    pcd += 4;
}

void compile_call()
{
    pcd_last_call = pcd;
    *pcd++ = 0xe8;		/* call near, 32 bit offset */
    compile_offset();
}

#if 0
void compile_jump()
{
    pcd_last_call = pcd;
    *pcd++ = 0xe9;		/* jump near, 32 bit offset */
    compile_offset();
}
#endif

void compile_return()
{
    if (pcd - 5 == pcd_last_call && *pcd_last_call == 0xe8)
	*pcd_last_call = 0xe9;	/* tail call --> jump */
    else
	*pcd++ = 0xc3;		/* ret near, don't pop any args */
}

void push_last_call()
{
    PUSH(&pcd_last_call);
}

void compile_stack_adjust(int n)
{
    int *help;

    help = (int *) pcd;
    help[0] = 0x0583;		/* addl $4,sp */
    help = (int *)(pcd + 2);
    help[0] = (int) &sp;
    help[1] = n & 0xff;		/* 8 bit offset */
    pcd += 7;
}
    
void compile_drop()
{
    compile_stack_adjust(4);
}

void compile_2drop()
{
    compile_stack_adjust(8);
}

/* not called directly; only called by generated code that first loads
 * a literal value into edx. */
static void push_literal()
{
    /* value is in edx */
    asm volatile ("movl sp,%eax; sub $4,sp; movl %edx,-4(%eax)");
}

void compile_literal_load()
{
    int *help;

    *pcd = 0xba;		/* mov immed to edx */
    help = (int *)(pcd + 1);
    help[0] = POP;		/* value */
    pcd += 5;
}

void compile_literal_push()
{
    PUSH((int) push_literal);
    compile_call();		/* sets pcd_last */
}

void compile_literal()
{
    compile_literal_load();
    compile_literal_push();
}

#ifdef NOTYET
void was_literal()
{
    if (pcd - 5 == pcd_last_call && *pcd_last_call == 0xe8
	&& (pcd + *(int *)(pcd - 4) == (char *) &push_literal))
    {
	/* last code compiled was a call to push_literal; back up,
           uncompile the call, and push the literal value onto the stack */
	/* XXX */
    }
}
#endif

void compile_sp_to_eax()
{
    int *help;

    *pcd = 0xa1;		/* movl sp,%eax */
    help = (int *)(pcd + 1);
    help[0] = (int) &sp;
    pcd += 5;
}

void compile_stack_move(int n)
{
    compile_sp_to_eax();
    compile_stack_adjust(n);
}
    
/* after struggling with gcc to create this code for me, I just did it
 * by hand. Sigh.
 */	

/* This requires that eax be loaded with sp, either with (for if) or
   without (for =if) a stack adjustment. */

/* After running out of "displacement space" in a Forth word, I've changed
   the branch compilers to use the 32-bit displacement versions. */

void compile_zbranch()
{
    int *help;

    help = (int *) pcd;
    help[0] = 0x0f003883; 	/* cmpl $0, (%eax); je rel32 */
    help[1] = 0x84;
    pcd += 9;
    PUSH(pcd);			/* 32 bit offset to fixup at (top)-1 */
}

void compile_branch()
{
    *pcd = 0xe9;		/* jmp rel32 unconditionally */
    pcd += 5;
    PUSH(pcd);			/* offset to fix up later */
}


/* We don't need 2shunt because we can call this as many times as
 * necessary; it only compiles 1 byte per call!
 */
void compile_shunt()		/* drop 4 bytes from r stack */
{
    *pcd++ = 0x58;		/* popl %eax */
}


/* Next challenge: for/next. These require popping and pushing the _other_
 * stack: the return (hardware) stack.
 */

void compile_pop_from_r()
{
    compile_stack_move(-4);	/* eax = sp; sp-- */
    *(int *) pcd = 0xfc408f;	/* popl -4(%eax) */
    pcd += 3;
    
}

void compile_2pop_from_r()
{
    int *help;

    compile_stack_move(-8);	/* eax = sp; sp -= 2 */
    help = (int *) pcd;
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

void compile_push_to_r()
{
    compile_stack_move(4);	/* eax = sp; sp++ */
    *(int *) pcd = 0x30ff;	/* pushl (%eax) */
    pcd += 2;
}

void compile_2push_to_r()
{
    int *help;

    compile_stack_move(8);	/* eax = sp; sp += 2 */
    help = (int *) pcd;
    help[0] = 0xff0470ff;	/* pushl 4(%eax); pushl (%eax) */
    help[1] = 0x30;
    pcd += 5;
}

void compile_copy_from_r()
{
    int *help;

    compile_stack_move(-4);	/* eax = sp; sp-- */
    help = (int *) pcd;
    help[0] = 0x8924148b;	/* movl (%esp),%edx; movl %edx,-4(%eax) */
    help[1] = 0xfc50;
    pcd += 6;
}

void compile_qfor()
{
    compile_stack_move(4);	/* eax = sp; sp++ */
    compile_zbranch();		/* cmpl $0, (%eax); je rel32 */
    *(int *) pcd = 0x30ff;	/* pushl (%eax) */
    pcd += 2;
}

void compile_next()
{
    int *help;

    help = (int *) pcd;
    help[0] = 0x0f240cff;	/* decl (%esp); jne <back32> */
    help[1] = 0x85;	
    help[2] = 0x5800;		/* popl %eax */
    pcd += 10;
    PUSH(pcd - 1);		/* for fixup of jne */
}

void dplus()
{
    asm volatile ("movl sp,%eax");
    asm volatile ("movl 4(%eax),%edx; addl %edx,12(%eax)");
    asm volatile ("movl 0(%eax),%edx; adcl %edx,8(%eax)");
    asm volatile ("addl $8,sp");
}

void dnegate()
{
    asm volatile ("movl sp,%eax");
    asm volatile ("negl (%eax); negl 4(%eax); sbbl $0,(%eax)");
}

void um_star()
{
    asm volatile ("movl sp,%ecx");
    asm volatile ("movl (%ecx),%eax; mull 4(%ecx)");
    asm volatile ("movl %edx,(%ecx); movl %eax,4(%ecx)");
}

void m_star()
{
    asm volatile ("movl sp,%ecx");
    asm volatile ("movl (%ecx),%eax; imull 4(%ecx)");
    asm volatile ("movl %edx,(%ecx); movl %eax,4(%ecx)");
}

void um_slash_mod()
{
    asm volatile ("movl sp,%ecx");
    asm volatile ("movl 4(%ecx),%edx; movl 8(%ecx),%eax"); /* dividend */
    asm volatile ("divl (%ecx)");	/* divisor */
    asm volatile ("movl %edx,8(%ecx); movl %eax,4(%ecx); addl $4,sp");
}

/*
    IDIV is symmetric.  To fix it (to make it _FLOOR_) we have to
    adjust the quotient and remainder when BOTH rem /= 0 AND
    the divisor and dividend are different signs.  (This is NOT the
    same as quot < 0, because the quot could be truncated to zero
    by symmetric division when the actual quotient is < 0!)
    The adjustment is q' = q - 1; r' = r + divisor.  This
    preserves the invariant  a / b => (r,q) s.t.  a = qb + r;
    a = q'b + r' = (q - 1)b + (r + b) = qb - b + r + b = qb + r !!
*/

void fm_slash_mod()		/* floored division! */
{
    asm volatile ("pushl %ebx");	/* callee saved! */
    asm volatile ("movl sp,%ecx");
    asm volatile ("movl 4(%ecx),%edx; movl 8(%ecx),%eax"); /* dividend */
    asm volatile ("idivl (%ecx)");	/* divisor */
    asm volatile ("movl (%ecx),%ebx; xorl 4(%ecx),%ebx");
    asm volatile ("jns 1f; orl %edx,%edx; je 1f; decl %eax; addl (%ecx),%edx");
    asm volatile ("1: movl %edx,8(%ecx); movl %eax,4(%ecx); addl $4,sp");
    asm volatile ("popl %ebx");		/* callee saved! */
}

/* for jumping thru a following "vector" of compiled calls */
void jump()
{
    asm volatile ("mov sp,%eax; addl $4,sp");

    /* edx = vector = return address; eax = index */
    asm volatile ("popl %edx; movl (%eax),%eax");

    /* edx points just _past_ the call/jmp instruction we're interested in */
    asm volatile ("leal 5(%eax,%eax,4),%eax; addl %eax,%edx");

    /* ... so back up and get the offset, and add it to edx; then go there! */
    asm volatile ("addl -4(%edx),%edx; jmp *%edx");
}
