/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* error-handling code for muForth */

#include "muforth.h"

#include <setjmp.h>
#include <stdlib.h>
#include <errno.h>

#ifndef XXX
#include <sys/uio.h>
#include <unistd.h>
#endif

#define write_string(f,s)  write(f,s,strlen(s))

/* XXX just use printf! */
/* XXX print out zloading and lineno too */

/* A bit of a crock, but only called if we haven't set up a catch frame. */
void die(const char *msg)
{
    write_string(2, "muforth: ");
    if (parsed.length != 0)
    {
        write(2, parsed.data, parsed.length);
        write(2, " ", 1);
    }
    write_string(2, msg);
    write(2, "\n", 1);
    exit(1);
}

/* just calls execute, then pushes 0 */
void mu_catch() { EXECUTE; PUSH(0); }

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
