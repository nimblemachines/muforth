/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2018 David Frech. (Read the LICENSE for details.)
 */

#include "muforth.h"
#include "version.h"

/* data stack */
cell  stack[STACK_SIZE];
cell  *SP;

/* return stack */
cell  rstack[STACK_SIZE];
cell  *RP;

xtk_cell  *IP;  /* instruction pointer */
xtk        W;   /* on entry, points to the current Forth word */

static void init_stacks()
{
    mu_sp_reset();
    RP = R0;
}

void mu_push_build_time()
{
    PUSH(BUILD_TIME);
}

void mu_push_build_date()
{
    PUSH_ADDR(BUILD_DATE);
    PUSH(strlen(BUILD_DATE));
}

void muforth_init()
{
    init_stacks();
    init_dict();
}

void muforth_start()
{
    PUSH_ADDR("startup.mu4");
    muboot_load_file();
    PUSH_ADDR("warm");      /* push the token "warm" */
    PUSH(4);
    muboot_interpret_token();   /* ... and execute it! */
}
