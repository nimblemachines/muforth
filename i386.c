/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright 1997-2004 David Frech. All rights reserved, and all wrongs
 * reversed.
 */

/* compiler essentials for intel architecture */

#include "muforth.h"

static u_int8_t *pcd_last_call;

static void mu_compile_offset()
{
    u_int8_t *dest = (u_int8_t *) POP;

    *(int *)pcd = dest - (pcd + 4);
    pcd += 4;
}

void mu_compile_call()
{
    pcd_last_call = pcd;
    *pcd++ = 0xe8;		/* call near, 32 bit offset */
    mu_compile_offset();
}

/* resolve a forward or backward jump - moved from startup.mu4 because it
 * was i386-specific. In this usage "src" points just _past_ the displacement
 * we need to fix up.
 * : resolve   ( src dest)  over -  swap cell- ! ( 32-bit displacement) ;
 */
void mu_resolve()
{
    int src = STK(1);
    int dest = TOP;
    int *psrc = (int *)src;
    psrc[-1] = dest - src;
    DROP(2);
}

#if 0
void mu_compile_jump()
{
    pcd_last_call = pcd;
    *pcd++ = 0xe9;		/* jump near, 32 bit offset */
    mu_compile_offset();
}
#endif

void mu_compile_return()
{
    if (pcd - 5 == pcd_last_call && *pcd_last_call == 0xe8)
	*pcd_last_call = 0xe9;	/* tail call --> jump */
    else
	*pcd++ = 0xc3;		/* ret near, don't pop any args */
}

void mu_push_last_call()
{
    PUSH(&pcd_last_call);
}

static void compile_stack_adjust(int n)
{
    int *help;

    help = (int *) pcd;
    help[0] = 0x0583;		/* addl $4,sp */
    help = (int *)(pcd + 2);
    help[0] = (int) &sp;
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

/* not called directly; only called by generated code that first loads
 * a literal value into edx. */
static void mu_push_literal()
{
    /* value is in edx */
    asm volatile ("movl sp,%eax; sub $4,sp; movl %edx,-4(%eax)");
}

void mu_compile_literal_load()
{
    int *help;

    *pcd = 0xba;		/* mov immed to edx */
    help = (int *)(pcd + 1);
    help[0] = POP;		/* value */
    pcd += 5;
}

void mu_compile_literal_push()
{
    PUSH((int) mu_push_literal);
    mu_compile_call();		/* sets pcd_last_call */
}

#ifdef NOTYET
void mu_was_literal()
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

static void mu_compile_sp_to_eax()
{
    int *help;

    *pcd = 0xa1;		/* movl sp,%eax */
    help = (int *)(pcd + 1);
    help[0] = (int) &sp;
    pcd += 5;
}

/* after struggling with gcc to create this code for me, I just did it
 * by hand. Sigh.
 */	

/* This requires that eax be loaded with sp, either with (for if) or
   without (for =if) a stack adjustment. */

/* After running out of "displacement space" in a Forth word, I've changed
   the branch compilers to use the 32-bit displacement versions. */

/* XXX: It might make sense to make _forward_ branches always 32 bits -
   since we don't know ahead of time how far we'll have to jump - but
   make backwards branches only as long as they need to be. */

static void compile_zbranch()
{
    int *help;

    help = (int *) pcd;
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
    *(int *) pcd = 0xfc408f;	/* popl -4(%eax) */
    pcd += 3;
    
}

void mu_compile_2pop_from_r()
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

void mu_compile_push_to_r()
{
    compile_stack_move(4);	/* eax = sp; sp++ */
    *(int *) pcd = 0x30ff;	/* pushl (%eax) */
    pcd += 2;
}

void mu_compile_2push_to_r()
{
    int *help;

    compile_stack_move(8);	/* eax = sp; sp += 2 */
    help = (int *) pcd;
    help[0] = 0xff0470ff;	/* pushl 4(%eax); pushl (%eax) */
    help[1] = 0x30;
    pcd += 5;
}

void mu_compile_copy_from_r()
{
    int *help;

    compile_stack_move(-4);	/* eax = sp; sp-- */
    help = (int *) pcd;
    help[0] = 0x8924148b;	/* movl (%esp),%edx; movl %edx,-4(%eax) */
    help[1] = 0xfc50;
    pcd += 6;
}

void mu_compile_qfor()
{
    compile_stack_move(4);	/* eax = sp; sp++ */
    compile_zbranch();		/* cmpl $0, (%eax); je rel32 */
    *(int *) pcd = 0x30ff;	/* pushl (%eax) */
    pcd += 2;
    PUSH(pcd);			/* push address to loop back to */
}

void mu_compile_next()
{
    int *help;

    help = (int *) pcd;
    help[0] = 0x0f240cff;	/* decl (%esp); jne <back32> */
    help[1] = 0x00000085;
    help[2] = 0x5800;		/* popl %eax */
    pcd += 10;
    PUSH(pcd - 1);		/* for fixup of jne */
}

void mu_dplus()
{
    asm volatile ("movl sp,%eax");
    asm volatile ("movl 4(%eax),%edx; addl %edx,12(%eax)");
    asm volatile ("movl 0(%eax),%edx; adcl %edx,8(%eax)");
    asm volatile ("addl $8,sp");
}

void mu_dnegate()
{
    asm volatile ("movl sp,%eax");
    asm volatile ("negl (%eax); negl 4(%eax); sbbl $0,(%eax)");
}

void mu_um_star()
{
    asm volatile ("movl sp,%ecx");
    asm volatile ("movl (%ecx),%eax; mull 4(%ecx)");
    asm volatile ("movl %edx,(%ecx); movl %eax,4(%ecx)");
}

void mu_m_star()
{
    asm volatile ("movl sp,%ecx");
    asm volatile ("movl (%ecx),%eax; imull 4(%ecx)");
    asm volatile ("movl %edx,(%ecx); movl %eax,4(%ecx)");
}

void mu_um_slash_mod()
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
    The adjustment is q' = q - 1; r' = r + divisor.

    This preserves the invariant a / b => (r,q) s.t. qb + r = a.

    q'b + r' = (q - 1)b + (r + b) = qb - b + r + b
             = qb + r
             = a

    where q',r' are the _floored_ quotient and remainder (really, modulus),
    and q,r are the symmetric quotient and remainder.

    XXX: discuss how the range of the rem/mod changes as the num changes
    (in symm. div.) and as the denom changes (in floored div).
*/

void mu_fm_slash_mod()		/* floored division! */
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
void mu_jump()
{
    asm volatile ("mov sp,%eax; addl $4,sp");

    /* edx = vector = return address; eax = index */
    asm volatile ("popl %edx; movl (%eax),%eax");

    /* edx points just _past_ the call/jmp instruction we're interested in */
    asm volatile ("leal 5(%eax,%eax,4),%eax; addl %eax,%edx");

    /* ... so back up and get the offset, and add it to edx; then go there! */
    asm volatile ("addl -4(%edx),%edx; jmp *%edx");
}
