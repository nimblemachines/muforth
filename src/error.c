/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* error-handling code for muForth */

#include "muforth.h"

#include <stdlib.h>     /* for exit() */
#include <errno.h>      /* XXX should this be sys/errno.h ? */
#include <stdio.h>

/* This string will always be in the Forth * heap, and so a cell-valued
 * "pointer" will work. */
static cell being_loaded = 0;   /* name of file currently being loaded */
void mu_push_being_loaded()     { PUSH_ADDR(&being_loaded); }

/* A bit of a crock, but only called if we haven't set up a catch frame. */
void die(const char* zmsg)
{
    fprintf(stderr, "%s, line %d: %.*s %s\n",
        being_loaded ? (char *)UNHEAPIFY(being_loaded) : "startup.mu4",
        parsed_lineno, (int)parsed.length, parsed.data, zmsg);
    exit(1);
}

/* abort() is deferred via this variable */
static cell xt_abort = 0;
void mu_push_tick_abort()   { PUSH_ADDR(&xt_abort); }


void mu_abort()     /* zmsg */
{
    if (xt_abort)
    {
        PUSH(xt_abort);
        mu_execute();
    }
    else
        die((char *)UNHEAPIFY(TOP));
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
