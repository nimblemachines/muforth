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
void mufs_readable_q()
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

void mufs_create_file()       /* C-string-name - fd */
{
    int fd;

    fd = open((char *)TOP, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd == -1)
    {
        throw_strerror();
    }

    TOP = fd;
}

void mufs_open_file()     /* C-string-name flags - fd */
{
    int fd;

    fd = open((char *)ST1, TOP);
    if (fd == -1)
    {
        throw_strerror();
    }
    DROP(1);
    TOP = fd;
}

void mufs_push_r_slash_o()
{
    PUSH(O_RDONLY);
}

void mufs_push_r_slash_w()
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

void mu_read_file()     /* fd - addr len */
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

    DROP(-1);
    ST1 = (cell) p;
    TOP = s.st_size;
}

void mu_read_carefully()    /* fd buffer len -- #read */
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

void mu_write_carefully()   /* fd buffer len */
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

/* Core io primitives */

/*
 * Every system must define the following functions, as a minimum for
 * interactive use:
 *
 *   typing     ( - addr len)
 *              read a line of input from user (console)
 *
 *   write      ( fd addr len)
 *              output a string to "file descriptor" - some platforms can
 *              ignore fd and simply write to the console
 *
 *   open-file  ( path - fd)
 *              find a copy of file based on path (platform-specific how
 *              the search happens); return a handle (fd) to refer to the
 *              file
 *
 *   read-file  ( fd - addr len)
 *              get the contents of the file (somehow) and return it as a
 *              string
 *
 *   close-file ( fd)
 *              release resources allocated to file
 */

void mu_typing()   /* -- inbuf #read */
{
    static char inbuf[1024];
    DROP(-4);
    ST3 = (cell)inbuf;
    ST2 = 0;                /* stdin */
    ST1 = (cell)inbuf;
    TOP = 1024;
    mu_read_carefully();    /* stdin inbuf size -- #read */
}

void mu_open_file()         /* c-path -- fd */
{
    mufs_push_r_slash_o();
    mufs_open_file();       /* c-path flags -- fd */
}

