/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2004 David Frech. All rights reserved, and all wrongs
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
#include <stdio.h>
#include <sys/types.h>
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

/* catch & throw? */
static jmp_buf *last_jb;	/* top of "stack" of jmp bufs */

/* This works like C and not like Forth: catch returns 0 when it sets up
 * the frame and non-zero when it has been longjumped to. Catch does _not_
 * call a function. But then, how/when do you undo the frame? Hmmm.
 * Okay, doing the Forth way. */

#define SETJMP setjmp
#define LONGJMP longjmp

void mu_catch()
{
    jmp_buf this_jb;
    jmp_buf *prev_jb;
    cell_t *saved_sp;
    cell_t thrown;

    prev_jb = last_jb;
    last_jb = &this_jb;
    saved_sp = sp;		/* so we can reset _our_ stack ptr */
    thrown = SETJMP(this_jb);
    if (thrown == 0)
	EXECUTE;
    else
	sp = saved_sp + 1;	/* we longjmp'ed; restore sp & pop xt */
    last_jb = prev_jb;
    PUSH(thrown);
}

void mu_throw()
{
    if (TOP != 0)
    {
	if (last_jb)
	    LONGJMP(*last_jb, TOP);
	else
	    die((char *)TOP);
    }
    DROP(1);
}

char *counted_strerror()
{
    return to_counted_string(strerror(errno));
}
