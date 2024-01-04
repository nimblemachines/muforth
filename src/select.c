/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* This is an interface to _select_. */

#include "muforth.h"

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

void mu_fd_reset()
{
    FD_ZERO((fd_set *) TOP);
    TOP = sizeof(fd_set);   /* so we can allot space in Forth */
}

void mu_fd_set()
{
    FD_SET(TOP, (fd_set *) ST1);
    DROP(2);
}

void mu_fd_clr()
{
    FD_CLR(TOP, (fd_set *) ST1);
    DROP(2);
}

void mu_fd_in_set_q()
{
    /* Return a Forth-compatible flag */
    ST1 = FD_ISSET(TOP, (fd_set *) ST1) ? -1 : 0;
    DROP(1);
}

void mu_select()
{
    int count;
    cell nfds = SP[4];
    fd_set *readfds = (fd_set *) ST3;
    fd_set *writefds = (fd_set *) ST2;
    fd_set *exceptfds = (fd_set *) ST1;
    struct timeval *timeout = (struct timeval *) TOP;

    DROP(4);
    while((count = select(nfds, readfds, writefds, exceptfds, timeout)) == -1)
    {
        if (errno == EINTR) continue;
        return abort_strerror();
    }
    TOP = count;
}
/*
  FD_ZERO could return the size of an FD! This way from Forth we can do:

  : fd-set  ram fd-zero  buffer ; (new-fd-set? create-fd-set?)

  and use that to create fd-sets.

  Then, how do you set an fd in an fd-set? The names get a little weird.
*/
