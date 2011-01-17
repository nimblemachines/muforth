/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

#include "muforth.h"
#include "version.h"

/* data stack */
cell stack[STACK_SIZE];
cell *SP;

/* return stack */
xtk *rstack[STACK_SIZE];
xtk **RP;

xtk   *IP;     /* instruction pointer */
xtk    W;      /* on entry, points to the current Forth word */

static void init_stacks()
{
    mu_sp_reset();
    RP = R0;
}

void mu_push_build_time()
{
#ifdef WITH_TIME
    PUSH(build_time);
#else
    PUSH(build_date);
    PUSH(strlen(build_date));
#endif
}

static void mu_start_up()
{
    PUSH("warm");       /* push the token "warm" */
    PUSH(4);
    muboot_interpret_token();    /* ... and execute it! */
}

void muforth_init()
{
    init_dict();
    init_stacks();
}

void muforth_start()
{
    PUSH("startup.mu4");
    muboot_load_file();
    mu_start_up();
}

