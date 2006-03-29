/*
 * $Id$
 *
 * This file is part of muforth.
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

cell *pcd_last_call;
cell *pcd_jump_dest;

/* Normal exit */
static void mu_exit()   { UNNEST; }

/* Tail-call exit */
static void mu_tail_call()  { W = *IP; UNNEST; (*W)(); }

pw p_mu_exit = &mu_exit;
pw p_mu_tail_call = &mu_tail_call;

void mu_literal_() { PUSH(*IP++); }

void mu_code_comma() { *pcd++ = POP; }

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

    /* also set up last jump destination, for tail-call code */
    pcd_jump_dest = (cell *)dest;
}

void _mu_compiler_exit()
{
    cell w;

    /* Convert call in tail position to jump. */
    /* Convert "word; EXIT" to "TAIL; word" */
    if (pcd == pcd_last_call && pcd != pcd_jump_dest)
    {
        w = pcd[-1];    /* last word compiled */
        pcd[-1] = (cell)XTK(mu_tail_call);
        *pcd++ = w;
    }
    else
        *pcd++ = (cell)XTK(mu_exit);
}

void mu_compiler_exit()
{
    *pcd++ = (cell)XTK(mu_exit);
}

void mu_set_colon_code() { *pcd++ = (cell)&mu_do_colon; }

void mu_set_does_code()  { *pcd++ = (cell)&mu_do_does; }

void mu_compile_comma()
{
    *pcd++ = POP;
    pcd_last_call = pcd;
}

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


