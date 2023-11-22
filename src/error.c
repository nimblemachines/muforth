/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2023 David Frech. (Read the LICENSE for details.)
 */

/* error-handling code for muForth */

#include "muforth.h"

#include <stdlib.h>     /* for exit() */
#include <errno.h>      /* XXX should this be sys/errno.h ? */
#include <stdio.h>

/* Shared with Forth code. */
static char *being_loaded = NULL;   /* name of file currently being loaded */

void mu_push_being_loaded()     { PUSH_ADDR(&being_loaded); }

/* A bit of a crock, but only called if we haven't set up a catch frame. */
void die(const char* zmsg)
{
    fprintf(stderr, "%s, line %d: %.*s %s\n",
        being_loaded ? being_loaded : "startup.mu4",
        parsed_lineno, (int)parsed.length, parsed.data, zmsg);
    exit(1);
}

/* abort() is deferred via this variable */
/* xt, being a pointer type, is an addr-sized, not cell-sized, value. */
static xt xt_abort = NULL;

void mu_abort()     /* zmsg */
{
    if (xt_abort)
    {
        /*
         * PUSH_ADDR because the contents of xt_abort is a machine
         * address.
         */
        PUSH_ADDR(xt_abort);
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
