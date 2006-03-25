/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
 * reversed.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * or see the file LICENSE in the top directory of this distribution.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
    T = thrown;
}

void mu_throw()
{
    if (T != 0)
    {
        if (last_tf)
            LONGJMP(last_tf->jb, T);
        else
            die((char *)T);
    }
    DROP;
}

char *counted_strerror()
{
    return to_counted_string(strerror(errno));
}
