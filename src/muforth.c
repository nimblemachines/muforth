/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2022 David Frech. (Read the LICENSE for details.)
 */

#include "muforth.h"
#include "version.h"

/* data stack */
val dstack[STACK_SIZE];

/* return stack */
val rstack[STACK_SIZE];

/* Forth VM execution registers */
val  *SP;    /* parameter stack pointer */
val  *RP;    /* return stack pointer */
xt   *IP;    /* instruction pointer; points to an xt */
xt    W;     /* on entry, points to the current Forth word */
 
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
