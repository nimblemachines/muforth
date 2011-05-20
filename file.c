/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2011 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* file primitives */

#include "muforth.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>     /* getenv */
#include <string.h>     /* memcpy */

/* XXX: for read, write; temporary? */
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

/* if found and readable, return -1
 * if not found or not readable, return 0
 */
static int readable(char *path)
{
    struct stat st;
    uid_t euid;
    gid_t egid;

    /* get our effective UID & GID for testing the file permissions */
    euid = geteuid();
    egid = getegid();

    if (stat(path, &st) == -1)
        return 0;   /* failed */

    /* test for user perms */
    if (st.st_uid == euid && (st.st_mode & 0400))
        return -1;  /* success */

    /* test for group perms */
    if (st.st_gid == egid && (st.st_mode & 0040))
        return -1;  /* success */

    /* test for world (other) perms */
    if (st.st_mode & 0004)
        return -1;  /* success */

    /* failed all tests, return false */
    return 0;
}

char *path_prefix(char *src, char *dest, char sep, char *begin)
{
    int len;

    /*
     * Checking dest lets us cascade calls without checking for NULL.
     * Checking src lets us pass results (say, of getenv) that could be
     * NULL.
     */
    if (dest == NULL || src == NULL) return NULL;      

    len = strlen(src);
    if (dest - len - 1 < begin) return NULL;
    *--dest = sep;      /* add path separator */
    dest -= len;        /* back up to make room for string at src */
    memcpy(dest, src, len);
    return dest;        /* new beginning */
}

static int try_path(char *p)
{
    if (p && readable(p)) return -1;
    return 0;
}

/*
 * find_file should look for the file (the c-path is a relative path) in
 * the following places, in order:
 *   ./path
 *   $MUFORTH/path
 *   $HOME/muforth/path
 * and nowhere else!
 */
static char* find_file(char *path, char *pathbuf, int bufsize)
{
    char *p;

    /* If path is absolute, just return it as is. */
    if (*path == '/') return path;

    /* start with path at end, if it fits! */
    path = path_prefix(path, pathbuf+bufsize, '\0', pathbuf);
    if (path == NULL) return NULL;

    /* now, for each prefix, push it onto front of path and try it */
    /* first, try bare path */
    if (try_path(path)) return path;

    /* next, try $MUFORTH/path */
    p = path_prefix(getenv("MUFORTH"), path, '/', pathbuf);
    if (try_path(p)) return p;

    /* last, try $HOME/muforth/path */
    p = path_prefix("muforth", path, '/', pathbuf);
    p = path_prefix(getenv("HOME"), p, '/', pathbuf);
    if (try_path(p)) return p;

    return NULL;    /* not found */
}

void mu_create_file()       /* C-string-name - fd */
{
    int fd;

    fd = open((char *)TOP, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd == -1)
        return abort_strerror();

    TOP = fd;
}

static void mu_open_file()     /* C-string-name flags - fd */
{
    int fd;
    char pathbuf[1024];
    char *path = find_file((char *)ST1, pathbuf, 1024);

    if (path == NULL)
        return abort_zmsg("file not found on search path");

    fd = open(path, TOP);
    if (fd == -1)
        return abort_strerror();

    DROP(1);
    TOP = fd;
}

void mu_open_file_ro()
{
    PUSH(O_RDONLY);
    mu_open_file();
}

void mu_open_file_rw()
{
    PUSH(O_RDWR);
    mu_open_file();
}

void mu_close_file()
{
    while (close(TOP) == -1)
    {
        if (errno == EINTR) continue;
        return abort_strerror();
    }
    DROP(1);
}

/*
 * mu_read_file mmaps the file and returns its contents as a string
 */
void mu_read_file()     /* fd - addr len */
{
    char *p;
    struct stat s;
    int fd;

    fd = TOP;

    if (fstat(fd, &s) == -1)
    {
        close(fd);
        return abort_strerror();
    }
    p = (char *) mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED)
    {
        close(fd);
        return abort_strerror();
    }

    DROP(-1);
    ST1 = (addr) p;
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
        return abort_strerror();
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
            return abort_strerror();
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
 *   open-file-ro  ( path - fd)  ( open read-only)
 *   open-file-rw  ( path - fd)  ( open read-write)
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
    ST3 = (addr)inbuf;
    ST2 = 0;                /* stdin */
    ST1 = (addr)inbuf;
    TOP = 1024;
    mu_read_carefully();    /* stdin inbuf size -- #read */
}
