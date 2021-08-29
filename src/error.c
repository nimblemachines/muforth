/*
 * This file is part of muforth: https://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2021 David Frech. (Read the LICENSE for details.)
 */

/* error-handling code for muForth */

#include "muforth.h"

#include <stdlib.h>     /* for exit() */
#include <errno.h>      /* XXX should this be sys/errno.h ? */
#include <stdio.h>

/* A bit of a crock, but only called if we haven't set up a catch frame. */
void die(const char* zmsg)
{
    fprintf(stderr, "startup.mu4, line %d: %.*s %s\n",
        parsed_lineno, (int)parsed.length, parsed.data, zmsg);
    exit(1);
}

/* abort() is deferred via this variable */
static xt_cell xt_abort = CELL(NULL);

void mu_abort()     /* zmsg */
{
    if (_(xt_abort))
    {
        /*
         * PUSH_ADDR because the contents of xt_abort are a machine
         * address.
         */
        PUSH_ADDR(_(xt_abort));
        mu_execute();
    }
    else
        die((char *)TOP);
}

void mu_push_tick_abort()
{
    PUSH_ADDR(&xt_abort);
}

/*
 * abort_zmsg() pushes a pointer to a zero-terminated string, and then
 * calls abort.
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
