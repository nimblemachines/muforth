/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2010 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* file primitives */

#include "muforth.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* XXX: for read, write; temporary? */
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

/* if found and readable, leave name and push -1;
 * if not found or not readable, drop name and push 0
 */
void mu_readable_q()
{
    struct stat st;
    uid_t euid;
    gid_t egid;

    /* get our effective UID & GID for testing the file permissions */
    euid = geteuid();
    egid = getegid();

    if (stat((char *)TOP, &st) == -1)
    {
        TOP = 0;    /* failed */;
        return;
    }

    /* test for user perms */
    if (st.st_uid == euid && (st.st_mode & 0400))
    {
        PUSH(-1);   /* success */
        return;
    }

    /* test for group perms */
    if (st.st_gid == egid && (st.st_mode & 0040))
    {
        PUSH(-1);   /* success */
        return;
    }

    /* test for world (other) perms */
    if (st.st_mode & 0004)
    {
        PUSH(-1);   /* success */
        return;
    }

    /* failed all tests, return false */
    TOP = 0;
}

void mu_create_file()       /* C-string-name - fd */
{
    int fd;

    fd = open((char *)TOP, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd == -1)
    {
        throw_strerror();
    }

    TOP = fd;
}

void mu_open_file()     /* C-string-name flags - fd */
{
    int fd;

    fd = open((char *)ST1, TOP);
    if (fd == -1)
    {
        throw_strerror();
    }
    NIP(1);
    TOP = fd;
}

void mu_push_r_slash_o()
{
    PUSH(O_RDONLY);
}

void mu_push_r_slash_w()
{
    PUSH(O_RDWR);
}

void mu_close_file()
{
    while (close(TOP) == -1)
    {
        if (errno == EINTR) continue;
        throw_strerror();
    }
    DROP(1);
}

void mu_mmap_file()     /* fd - addr len */
{
    char *p;
    struct stat s;
    int fd;

    fd = TOP;

    if (fstat(fd, &s) == -1)
    {
        close(fd);
        throw_strerror();
    }
    p = (char *) mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED)
    {
        close(fd);
        throw_strerror();
    }

    NIP(-1);
    ST1 = (cell) p;
    TOP = s.st_size;
}

/* NOTE: These two routines will be obsoleted by buf_* routines. */
void mu_read_carefully()
{
    int fd;
    char *buffer;
    size_t len;
    ssize_t count;

    fd = ST2;
    buffer = (char *) ST1;
    len = TOP;
    DROP(2);

    while((count = read(fd, buffer, len)) == -1)
    {
        if (errno == EINTR) continue;
        throw_strerror();
    }
    TOP = count;
}

void mu_write_carefully()
{
    int fd;
    char *buffer;
    size_t len;
    ssize_t written;

    fd = ST2;
    buffer = (char *) ST1;
    len = TOP;
    DROP(3);

    while (len > 0)
    {
        written = write(fd, buffer, len);
        if (written == -1)
        {
            if (errno == EINTR) continue;
            throw_strerror();
        }
        buffer += written;
        len -= written;
    }
}
