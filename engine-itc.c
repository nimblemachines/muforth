/*
 * $Id$
 *
 * This file is part of muFORTH; for project details, visit
 *
 *    http://nimblemachines.com/browse?show=MuForth
 *
 * Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
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

/* Nuts and bolts of threaded-code compiler & execution engine */

#include "muforth.h"

/*
 * ITC code pointers, so we can create XTKs for these words. Their code
 * pointers also exist in the dictionary, but we'd have to query for them
 * and still store them somewhere. This is easier.
 */
pw p_mu_nope = &mu_nope;
pw p_mu_interpret = &mu_interpret;
pw p_mu_evaluate = &mu_evaluate;
pw p__mu__lbracket = &_mu__lbracket;
pw p__mu__rbracket = &_mu__rbracket;

void mu_compile_comma()  { *pcd++ = POP; }
void mu_set_colon_code() { *pcd++ = (cell)&mu_do_colon; }
void mu_set_does_code()  { *pcd++ = (cell)&mu_do_does; }

/*
 * This is where all the magic happens. Any time we have the address of a
 * word to execute (an execution token, or XTK) - either because we looked
 * it up in the dictionary, or because we fetched it out of a variable - we
 * call execute_xtk. It is most commonly invoked by EXECUTE, which POPs an
 * xtk off the D stack, and calls execute_xtk.
 *
 * We need magic here because of the hybrid nature of the system. It is
 * *not* a truly indirect-threaded Forth, where you set up IP and go, and
 * never look back. We're executing C code, then Forth, then C... The Forth
 * inner interpreter never runs "free"; in fact, we run it in this routine!
 *
 * How do we know if the XTK refers to a Forth word or to a C routine? We
 * don't...until we run it. If it's Forth, the first thing it will do is
 * NEST - push the caller's IP - and then it will set IP to point to its
 * own code. In fact, that's *all* Forth words do when you execute them!
 *
 * So we save RP, then call the word. Then we loop *while* the current RP
 * is *strictly* less than the saved. If we didn't call a Forth word, this
 * will not be true, and we'll exit. But the word we called - the C routine
 * - will have run its course and done all its work.
 *
 * If it *was* a Forth word, it NESTED, and RP < rp_saved. So now we run
 * NEXT - a "constrained" threaded interpreter - until RP >= rp_saved, which
 * will happen precisely when the called (Forth) word executes UNNEST
 * (EXIT).
 *
 * That's all there is to it!
 *
 * Thanks to Michael Pruemm for the idea of comparing RP to rp_saved as a
 * way to see when we're "done".
 */
void execute_xtk(xtk x)
{
    xtk **rp_saved;

    rp_saved = RP;

    CALL(x);
    while (RP < rp_saved)
        NEXT;
}

/* The most important "word" of all: */
void mu_do_colon()
{
    NEST;                   /* entering a new word; push IP */
    IP = (xtk *)&W[1];      /* new IP is address of parameter field */
}

/* The basis of create/does>. */
void mu_do_does()
{
    NEST;                   /* entering a new word; push IP */
    IP = (xtk *)W[1];       /* new IP is stored in the parameter field */
    PUSH(W[2]);             /* push the constant stored in 2nd word */
}

/* Normal exit */
void mu_exit()      { UNNEST; }

/* Push an inline literal */
void mu_literal_()  { PUSH(*IP++); }

/*
 * These are the control structure runtime workhorses. They are static
 * because we don't export them directly to muforth; instead we export
 * words that *compile* them.
 */
static void mu_branch_()            { BRANCH; }
static void mu_equal_zero_branch_() { if (TOP == 0) BRANCH; else IP++; }
static void mu_zero_branch_()       { mu_equal_zero_branch_(); DROP(1); }

/* for, ?for, next */
/* for is simply "push" */
/* ?for has to matched with "then" */
static void mu_qfor_()
{
    if (TOP == 0)
    {
        BRANCH;
        DROP(1);
    }
    else
    {
        IP++;
        RPUSH(POP);
    }
}

static void mu_next_()
{
    cell *prtop;
    prtop = (cell *)RP;

    if (--*prtop == 0)          /* decrement top of R stack */
    {
        IP += 1;                /* skip branch */
        RP += 1;                /* pop counter */
    }
    else
    {
        BRANCH;                 /* take branch */
    }
}

/*
 * Since we're now compiling control structure bits in this file, we need
 * some more ITC code pointers. Since the "workhorse" words aren't even
 * going into the dictionary, we definitely need these code pointers!
 */

/*
 * For compiling control structures, we need architecture-specific help. We
 * don't know, eg, how long displacements are, and we don't know how to
 * resolve branches. So we isolate the specifics here, and put the
 * generalities in startup.mu4.
 */

/*
 * Mark a branch source for later fixup.
 * : mark>  (s - src)  here  0 code, ;
 */
void mu_mark_forward()
{
    PUSH(pcd);  /* leave the address of the following 0 */
    *pcd++ = 0;
}

/*
 * Resolve a forward or backward jump, from src to dest.
 */
void mu_resolve_forward()  /* src dest - */
{
    /* In ITC land, this is easy: just store dest at src. */
    cell *src = (cell *)ST1;
    cell dest = TOP;
    *src = dest;
    DROP(2);
}

enum mu_branch_t { BRANCH_T, ZBRANCH_T, EQZBRANCH_T, FOR_T, QFOR_T, NEXT_T };

static pw p_mu_branches[] = {
    &mu_branch_,
    &mu_zero_branch_,
    &mu_equal_zero_branch_,
    &mu_push,
    &mu_qfor_,
    &mu_next_
};

static void compile_branch(enum mu_branch_t br)
{
    *pcd++ = (cell)&p_mu_branches[br];
}

/*
 * Words that *compile* the above control structure words.
 */
void mu_branch_comma()
{
    compile_branch(BRANCH_T);
}

void mu_zero_branch_comma()      
{
    compile_branch(ZBRANCH_T);
}

void mu_equal_zero_branch_comma()
{
    compile_branch(EQZBRANCH_T);
}

void mu_for_comma()
{
    compile_branch(FOR_T);
}

void mu_qfor_comma()
{
    compile_branch(QFOR_T);
}

void mu_next_comma()
{
    compile_branch(NEXT_T);
}

   
/*
 * Runtime workhorses for R stack functions. In the x86 native code, "push"
 * and "pop" are compiler words. Here they don't need to be.
 */

/* R stack functions */
void mu_push()   { RPUSH(POP); }
void mu_pop()    { PUSH(RPOP); }
void mu_rfetch() { PUSH(RP[0]); }

void mu_shunt()  {  RP++; }
