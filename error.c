/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2009 David Frech. All rights reserved, and all wrongs
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

/* This works like C and not like Forth: catch returns 0 when it sets up
 * the frame and non-zero when it has been longjumped to. Catch does _not_
 * call a function. But then, how/when do you undo the frame? Hmmm.
 * Okay, doing it the Forth way. */

#define SETJMP setjmp
#define LONGJMP longjmp

struct trapframe
{
    jmp_buf jb;
    struct trapframe *prev;
    xtk *ip;
    xtk **rp;
};

static struct trapframe *last_tf;    /* top of "stack" of trap frames */

void mu_catch()
{
    struct trapframe tf;
    cell thrown;

    /* link onto front of trapframe list */
    tf.prev = last_tf;
    last_tf = &tf;

    tf.rp = RP;
    tf.ip = IP;

    thrown = SETJMP(tf.jb);
    if (thrown == 0)
    {
        EXECUTE;
        DUP;     /* make room for thrown value */
    }
    else
    {
        RP = tf.rp;     /* we longjmp'ed; restore rp & ip */
        IP = tf.ip;
    }
    /* unlink frame */
    last_tf = tf.prev;

    /* return thrown value */
    TOP = thrown;
}

void mu_throw()
{
    if (TOP != 0)
    {
        if (TOP == -1)        /* generic error value */
            TOP = (cell)"Unspecified fatal error";

        if (last_tf)
            LONGJMP(last_tf->jb, TOP);
        else
            die((char *)TOP);
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
