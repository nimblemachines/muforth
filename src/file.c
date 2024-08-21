/*
 * This file is part of muforth: https://muforth.dev/
 *
 * Copyright (c) 2002-2024 David Frech. (Read the LICENSE for details.)
 */

/* file primitives */

#include "muforth.h"
#include "muhome.h"     /* MU_DIR */

#include <limits.h>     /* PATH_MAX */
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* XXX: for read, write; temporary? */
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

/*
 * This is my version of stpcpy()
 *
 * I'm defining my own just in case the C library we are linking with lacks
 * stpcpy() - and because I already had this function when I discovered the
 * *existence* of stpcpy(). ;-).
 *
 * Unlike memcpy, which STOOPIDLY returns dest, rather than something
 * USEFUL, this routine copies strlen(src) bytes to dest, appends a null,
 * and then returns a pointer to the null terminator.
 */
char *string_copy(char *dest, char *src)
{
    size_t length = strlen(src);

    memcpy(dest, src, length);
    dest += length;
    *dest = '\0';
    return dest;
}

/*
 * Concatenate two paths, separated by a '/' character, and return a
 * pointer to dest. If the result is too long to fit in dest, return NULL.
 *
 * NOTE: Because we return dest and not a pointer to the null at the end,
 * this routine is not as useful for *cascading*, but it's good for other
 * uses, because its return value can be returned *directly* to a caller.
 */
char *concat_paths(char *dest, size_t destsize, char *p1, char *p2)
{
    if (strlen(p1) + strlen(p2) + 2 > destsize)
        return NULL;

    string_copy(string_copy(string_copy(dest, p1), "/"), p2);
    return dest;
}

/*
 * Convert path to an absolute path.
 *
 * If path starts with "/", return it unchanged.
 * If path starts with "./", prefix it with the current directory.
 * Otherwise, prefix it with the muforth "home" directory MU_DIR, which is
 * usually something like "/home/<user>/muforth/mu/". (The build process
 * sets it automagically.)
 */
static char* abs_path(char *dest, size_t destsize, char *path)
{
    /* If path is absolute, just return it as is. */
    if (path[0] == '/') return path;

    /*
     * If path starts with "./", strip the "./" and then prepend the
     * current working dir. If getcwd fails, or if the resulting path is
     * too long, return NULL.
     */
    if (path[0] == '.' && path[1] == '/')
    {
        char pwd[PATH_MAX];
        if (getcwd(pwd, PATH_MAX) == NULL) return NULL;
        return concat_paths(dest, destsize, pwd, path + 2);
    }

    /* Otherwise, rewrite path as MU_DIR/path */
    return concat_paths(dest, destsize, MU_DIR, path);
}

#ifdef WITH_STAT

void mu_stat_file()         /* C-string-name - mode-bits -1 | strerror 0 */
{
    struct stat s;
    char pathbuf[PATH_MAX];
    char *path = abs_path(pathbuf, PATH_MAX, (char *)UNHEAPIFY(TOP));

    if (path == NULL)
        return abort_zmsg("path too long");

    DROP(-1);
    if (stat(path, &s) == -1)
    {
        ST1 = (addr)strerror(errno);
        TOP = 0;        /* failure */
    }
    else
    {
        ST1 = s.st_mode;
        TOP = -1;       /* success */
    }
}

#endif  /* WITH_STAT */

void mu_create_file()       /* C-string-name - fd */
{
    int fd;
    char pathbuf[PATH_MAX];
    char *path = abs_path(pathbuf, PATH_MAX, (char *)UNHEAPIFY(TOP));

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
    char pathbuf[PATH_MAX];
    char *path = abs_path(pathbuf, PATH_MAX, (char *)UNHEAPIFY(ST1));

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

void mu_open_file_wo()
{
    PUSH(O_WRONLY);
    mu_open_file();
}

void mu_open_file_rw()
{
    PUSH(O_RDWR);
    mu_open_file();
}

/* For reading and writing pipes. */
void mu_open_file_wo_nonblocking()
{
    PUSH(O_WRONLY | O_NONBLOCK);
    mu_open_file();
}

void mu_open_file_ro_nonblocking()
{
    PUSH(O_RDONLY | O_NONBLOCK);
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
    ST1 = HEAPIFY(p);
    TOP = s.st_size;
}

ssize_t read_carefully(int fd, void *buffer, size_t len)
{
    ssize_t count;

    while((count = read(fd, buffer, len)) == -1)
    {
        if (errno == EINTR || errno == EAGAIN) continue;
        abort_strerror();
        return 0;
    }
    return count;
}

void write_carefully(int fd, void *buffer, size_t len)
{
    ssize_t written;

    while (len > 0)
    {
        written = write(fd, buffer, len);
        if (written == -1)
        {
            if (errno == EINTR || errno == EAGAIN) continue;
            return abort_strerror();
        }
        buffer += written;
        len -= written;
    }
}

void mu_read()      /* fd buffer len -- #read */
{
    int fd = ST2;
    void *buffer = (void *)UNHEAPIFY(ST1);
    size_t len = TOP;

    DROP(2);
    TOP = read_carefully(fd, buffer, len);
}

void mu_write()     /* fd buffer len */
{
    int fd = ST2;
    void *buffer = (void *)UNHEAPIFY(ST1);
    size_t len = TOP;

    DROP(3);
    write_carefully(fd, buffer, len);
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
