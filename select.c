/* This is an interface to _select_. */

#include "muforth.h"

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

void my_fd_zero()
{
    FD_ZERO((fd_set *) TOP);
    TOP = sizeof(fd_set);	/* so we can allot space in Forth */
}

void my_fd_set()
{
    FD_SET(TOP, (fd_set *) STK(1));
    DROP(2);
}

void my_fd_clr()
{
    FD_CLR(TOP, (fd_set *) STK(1));
    DROP(2);
}

void my_fd_isset()
{
    /* Return a Forth-compatible flag */
    STK(1) = FD_ISSET(TOP, (fd_set *) STK(1)) ? -1 : 0;
    DROP(1);
}

void my_select()
{
    int count;
    int nfds = STK(4);
    fd_set *readfds = (fd_set *) STK(3);
    fd_set *writefds = (fd_set *) STK(2);
    fd_set *exceptfds = (fd_set *) STK(1);
    struct timeval *timeout = (struct timeval *) TOP;

    DROP(4);
    for (;;)
    {
	count = select(nfds, readfds, writefds, exceptfds, timeout);
	if (count == -1)
	{
	    if (errno == EINTR) continue;
	    TOP = (int) strerror(errno);
	    throw();
	}
	break;
    }
    TOP = count;
}
/*
FD_ZERO could return the size of an FD! This way from Forth we can do:

: fd-set  ram fd-zero  buffer ; (new-fd-set? create-fd-set?)

and use that to create fd-sets.

Then, how do you set an fd in an fd-set? The names get a little weird.
*/
