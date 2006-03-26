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

void mu_compile_exit()
{
    PUSH(&p_mu_exit);
    mu_compile_comma();
}

void mu_set_colon_code() { *pcd++ = (cell)&mu_do_colon; }

void mu_set_does_code()  { *pcd++ = (cell)&mu_do_does; }

void mu_compile_comma() { *pcd++ = POP; }


/* Thanks to Michael Pruemm for the idea of comparing RP to rp_saved as a
 * way to see when we're "done".
 */
static void run()
{
    xtk **rp_saved = RP;

    while (RP <= rp_saved)
        NEXT;
}

/* The most important "word" of all: */
void mu_do_colon() { NEST; IP = (xtk *)&W[1]; run(); }

/* The base of create/does>. */
void mu_do_does() { NEST; IP = (xtk *)W[1]; PUSH(W[2]); run(); }

void mu_exit()   { UNNEST; }

pw p_mu_exit = &mu_exit;

void mu_literal_() { PUSH(*IP++); }


