/*
 * This file is part of muFORTH: http://pages.nimblemachines.com/muforth
 *
 * Copyright (c) 1997-2008 David Frech. All rights reserved, and all wrongs
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
    cell *sp;
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

    tf.sp = SP;
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
        SP = tf.sp;  /* we longjmp'ed; restore sp, rp, & ip */
        RP = tf.rp;
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
        if (last_tf)
            LONGJMP(last_tf->jb, TOP);
        else
            die((char *)TOP);
    }
    DROP(1);
}

char *counted_strerror()
{
    return to_counted_string(strerror(errno));
}
