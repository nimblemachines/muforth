/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* Quick and dirty pty - pseudo-tty - support */

/* Totally stupid GNU/Linux bullshit!! */
#ifdef __linux__
#define _XOPEN_SOURCE 600
#endif

#include "muforth.h"

#include <stdlib.h>
#include <fcntl.h>

/* Leaves fd of master, and pathname to slave device. */
void mu_open_pty()      /* ( - fd z") */
{
    int fd;         /* fd of master */
    char *slave;    /* name of corresponding slave device */

    fd = posix_openpt(O_RDWR);
    if (fd == -1)
        return abort_strerror();

    if (grantpt(fd) == -1)
        return abort_strerror();

    if (unlockpt(fd) == -1)
        return abort_strerror();

    slave = ptsname(fd);
    if (slave == NULL)
        return abort_strerror();

    PUSH(fd);
    PUSH_ADDR(slave);
}
