/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2016 David Frech. (Read the LICENSE for details.)
 */

/* error-handling code for muForth */

#include "muforth.h"

#include <stdlib.h>     /* for exit() */
#include <errno.h>
#include <stdio.h>

/* A bit of a crock, but only called if we haven't set up a catch frame. */
void die(const char* zmsg)
{
    fprintf(stderr, "startup.mu4, line %d: %.*s %s\n",
        parsed_lineno, (int)parsed.length, parsed.data, zmsg);
    exit(1);
}

void muboot_die()      /* zmsg */
{
    die((char *)TOP);
}

/* abort() is deferred via this variable */
CODE(muboot_die)
static xtk_cell xtk_abort = XTK(muboot_die);

void mu_abort()     /* zmsg */
{
    execute_xtk(_(xtk_abort));
}

void mu_push_tick_abort()
{
    PUSH_ADDR(&xtk_abort);
}

/*
 * abort_zmsg() is shorthand for PUSH(zmsg); mu_abort();
 */

void abort_zmsg(const char *zmsg)
{
    PUSH_ADDR(zmsg);
    mu_abort();
}

void assert(int cond, const char *zmsg)
{
    if (!cond) return abort_zmsg(zmsg);
}

/*
 * We've returned to using C strings for abort()ing. Since all we ever do
 * with strerror strings is abort(), passing them, let's roll the two ideas
 * into one.
 */
void abort_strerror()
{
    abort_zmsg(strerror(errno));
}
