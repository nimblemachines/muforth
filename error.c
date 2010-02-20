/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* error-handling code for muForth */

#include "muforth.h"

#include <stdlib.h>     /* for exit() */
#include <errno.h>
#include <stdio.h>

/* A bit of a crock, but only called if we haven't set up a catch frame. */
void die(const char *msg)
{
    int length = first - token_first - 1;

    fprintf(stderr, "startup.m4, line %d: %.*s %s\n",
            token_lineno, length, token_first, msg);
    exit(1);
}

/* just calls die */
void mu_throw()
{
    char *zmsg;

    if (TOP != 0)
    {
        if (TOP == -1)        /* generic error value */
            zmsg = "Unspecified fatal error";
        else
            zmsg = (char *)TOP;

        die((char *)zmsg);
    }
    DROP(1);
}

/*
 * throw() is shorthand for PUSH(zstring); mu_throw();
 *
 * Since we're going to reset the SP anyway, why not always do a
 * PUSH(zstring) and then mu_throw()? In other words, don't worry about
 * whether it's ok to simply assign TOP to the zstring. Let's always PUSH,
 * and let catch reset the stack.
 *
 * This code assumes that zstring != 0. Otherwise, the assumption that we
 * can always PUSH is false, since throw()ing 0 doesn't change the stack.
 */

void throw(const char *zstring)
{
    PUSH(zstring);
    mu_throw();
}

/*
 * We've returned to using C strings for throw()ing. Since all we ever do
 * with strerror strings is throw() them, let's roll the two ideas into
 * one.
 */
void throw_strerror()
{
    throw(strerror(errno));
}
