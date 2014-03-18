/*
 * This file is part of muFORTH: http://muforth.nimblemachines.com/
 *
 * Copyright (c) 2002-2014 David Frech. All rights reserved, and all wrongs
 * reversed. (See the file COPYRIGHT for details.)
 */

/* file primitives */

#include "muforth.h"
#include "version.h"        /* MU_DIR */

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>     /* memcpy */

/* XXX: for read, write; temporary? */
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

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

#ifndef WITH_CURRENT_DIR

/*
 * Convert path to an absolute path. If path starts with "/", then return
 * it unchanged; otherwise, prefix path with the muforth build directory.
 */
static char* abs_path(char *path, char *pathbuf, int bufsize)
{
    /* If path is absolute, just return it as is. */
    if (*path == '/') return path;

    /* Otherwise, rewrite path as MU_DIR/path */
    path = path_prefix(path, pathbuf+bufsize, '\0', pathbuf);
    return path_prefix(MU_DIR, path, '/', pathbuf);
}

#else

/*
 * NOTE: I think this is too complicated, which is why it isn't compiled by
 * default, but if you really want it, define WITH_CURRENT_DIR and
 * recompile.
 *
 * Convert path to an absolute path.
 * If path starts with "/", return it unchanged.
 * If path starts with ".", prefix it with the current directory.
 * Otherwise, prefix it with the muforth build directory.
 */
static char* abs_path(char *path, char *pathbuf, int bufsize)
{
    /* If path is absolute, just return it as is. */
    if (*path == '/') return path;

    {
        char *p;

        /* Get ready to add a path prefix. */
        p = path_prefix(path, pathbuf+bufsize, '\0', pathbuf);

        /* If path starts with ".", prepend the current working dir. */
        if (*path == '.')
        {
            char pwd[1024];
            if (getcwd(pwd, 1024) == NULL) return NULL;
            return path_prefix(pwd, p, '/', pathbuf);
        }

        /* Otherwise, rewrite path as MU_DIR/path */
        return path_prefix(MU_DIR, p, '/', pathbuf);
    }
}

/* For testing. Easiest way to use it:
 *   pad cwd type
 */
void mu_push_cwd()   /* addr - addr count */
{
    char *dest = (char *)TOP;

    if (getcwd(dest, 1024) == NULL)
        PUSH(0);
    else
        PUSH(strlen(dest));
}

#endif  /* WITH_CURRENT_DIR */

void mu_create_file()       /* C-string-name - fd */
{
    int fd;
    char pathbuf[1024];
    char *path = abs_path((char *)TOP, pathbuf, 1024);

    if (path == NULL)
        return abort_zmsg("path too long");

    fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd == -1)
        return abort_strerror();

    TOP = fd;
}

static void mu_open_file()     /* C-string-name flags - fd */
{
    int fd;
    char pathbuf[1024];
    char *path = abs_path((char *)ST1, pathbuf, 1024);

    if (path == NULL)
        return abort_zmsg("path too long");

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
    char *p = NULL;
    struct stat s;
    int fd;

    fd = TOP;

    if (fstat(fd, &s) == -1)
    {
        close(fd);
        return abort_strerror();
    }

    /* If size of file is zero, don't try to mmap; it will fail and error
     * out. Instead, simply return a buffer starting at address 0, of
     * length 0.
     */
    if (s.st_size != 0)
    {
        p = (char *) mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (p == MAP_FAILED)
        {
            close(fd);
            return abort_strerror();
        }
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
 *
 *
 * Defining the following is optional. If it doesn't exist in C, it will be
 * defined in Forth, either in a simple form (if no termios support is
 * available in the system) or in a more sophisticated form with
 * command-line editing and history (if termios support is available).
 *
 *   typing     ( - addr len)
 *              read a line of input from user (console)
 */
