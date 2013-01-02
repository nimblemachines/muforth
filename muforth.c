/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2013 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
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
#ifdef WITH_TIME
    PUSH(BUILD_TIME);
#else
    PUSH_ADDR(BUILD_DATE);
    PUSH(strlen(BUILD_DATE));
#endif
}

#ifdef FAKE_TTY_WIDTH
void mu_tty_width()  { TOP = 80; }  /* fd -- width */
#endif

static void mu_start_up()
{
    PUSH_ADDR("warm");       /* push the token "warm" */
    PUSH(4);
    muboot_interpret_token();    /* ... and execute it! */
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
    mu_start_up();
}
