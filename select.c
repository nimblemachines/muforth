/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2005 David Frech. All rights reserved, and all wrongs
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

/* This is an interface to _select_. */

#include "muforth.h"

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

void mu_fd_zero()
{
    FD_ZERO((fd_set *) T);
    T = sizeof(fd_set);   /* so we can allot space in Forth */
}

void mu_fd_set()
{
    FD_SET(T, (fd_set *) SND);
    DROP2;
}

void mu_fd_clr()
{
    FD_CLR(T, (fd_set *) SND);
    DROP2;
}

void mu_fd_isset()
{
    /* Return a Forth-compatible flag */
    SND = FD_ISSET(T, (fd_set *) SND) ? -1 : 0;
    DROP;
}

void mu_select()
{
    int count;
    cell nfds = SP[3];
    fd_set *readfds = (fd_set *) SP[2];
    fd_set *writefds = (fd_set *) TRD;
    fd_set *exceptfds = (fd_set *) SND;
    struct timeval *timeout = (struct timeval *) T;

    NIPN(4);
    for (;;)
    {
        count = select(nfds, readfds, writefds, exceptfds, timeout);
        if (count == -1)
        {
            if (errno == EINTR) continue;
            T = (cell) counted_strerror();
            mu_throw();
        }
        break;
    }
    T = count;
}
/*
  FD_ZERO could return the size of an FD! This way from Forth we can do:

  : fd-set  ram fd-zero  buffer ; (new-fd-set? create-fd-set?)

  and use that to create fd-sets.

  Then, how do you set an fd in an fd-set? The names get a little weird.
*/
