/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

#include "muforth.h"
#include "version.h"

/* data stack */
cell dstack[STACK_SIZE];

/* return stack */
cell rstack[STACK_SIZE];

/* Forth VM execution registers */
cell  *SP;  /* parameter stack pointer */
cell  *RP;  /* return stack pointer */
cell  *IP;  /* instruction pointer; points to an xt */
cell  *W;   /* on entry, points to the current Forth word */

static void init_stacks()
{
    mu_sp_reset();
    RP = RP0;
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
