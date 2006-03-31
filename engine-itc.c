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

/* Nuts and bolts of threaded-code compiler */

#include "muforth.h"

/* Normal exit */
void mu_exit()      { UNNEST; }

void mu_literal_()  { PUSH(*IP++); }

/* These are the same for the ITC engine, but aren't necessarily the same. */
void mu_compile_comma()  { *pcd++ = POP; }
void mu_code_comma()     { *pcd++ = POP; }

/*
 * Mark a branch source for later fixup.
 * : mark>  (s - src)  here  0 code, ;
 */
void mu_mark_from()
{
    PUSH(pcd);  /* leave the address of the following 0 */
    *pcd++ = 0;
}

/*
 * Resolve a forward or backward jump, from src to dest.
 */
void mu_resolve()  /* src dest - */
{
    /* In ITC land, this is easy: just store dest at src. */
    cell *src = (cell *)ST1;
    cell dest = TOP;
    *src = dest;
    DROP(2);
}

void mu_set_colon_code() { *pcd++ = (cell)&mu_do_colon; }

void mu_set_does_code()  { *pcd++ = (cell)&mu_do_does; }

/* Thanks to Michael Pruemm for the idea of comparing RP to rp_saved as a
 * way to see when we're "done".
 */

void execute(xtk x)
{
    xtk **rp_saved;

    rp_saved = RP;

    EXEC(x);
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

/*
 * "kernel" routines whose definition is specific to the implementation
 * type. So we'll implement these differently in the native version.
 */

/*
 * For compiling control structures, we need architecture-specific help. We
 * don't know, eg, how long displacements are, and we don't know how to
 * resolve branches. So we isolate the specifics here, and put the
 * generalities in startup.mu4.
 */

/*
 * To bootstrap ourselves, we need "if".
 * : if    (s - src)   (0branch)  mark> ;
 */

/*
 * These do_ words are the workhorses.
 */
void mu_branch_()            { BRANCH; }
void mu_equal_zero_branch_() { if (TOP == 0) BRANCH; else IP++; }
void mu_zero_branch_()       { mu_equal_zero_branch_(); DROP(1); }

/* for, ?for, next */
/* for is simply "push" */
/* ?for has to matched with "then" */
void mu_qfor_()
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

void mu_next_()
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
 * Similarly for R stack functions. In the x86 native code, "push" and
 * "pop" are compiler words. Here they don't need to be.
 */
/* r stack functions */
void mu_push()   { RPUSH(POP); }
void mu_pop()    { PUSH(RPOP); }
void mu_rfetch() { PUSH(RP[0]); }

void mu_shunt()  {  RP++; }
