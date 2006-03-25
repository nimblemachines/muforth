/*
 * $Id$
 *
 * This file is part of muforth.
 *
 * Copyright (c) 1997-2006 David Frech. All rights reserved, and all wrongs
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

    if (stat((char *)T, &st) == -1)
    {
        T = 0;    /* failed */;
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
    T = 0;
}

void mu_create_file()       /* C-string-name - fd */
{
    int fd;

    fd = open((char *)T, O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if (fd == -1)
    {
        T = (cell) counted_strerror();
        mu_throw();
    }
    T = fd;
}

void mu_open_file()     /* C-string-name flags - fd */
{
    int fd;

    fd = open((char *)SND, T);
    if (fd == -1)
    {
        T = (cell) counted_strerror();
        mu_throw();
    }
    NIP;
    T = fd;
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
    for (;;)
    {
        if (close(T) == -1)
        {
            if (errno == EINTR) continue;
            T = (cell) counted_strerror();
            mu_throw();
        }
        break;
    }
    DROP;
}

void mu_mmap_file()     /* fd - addr len */
{
    char *p;
    struct stat s;
    int fd;

    fd = T;

    if (fstat(fd, &s) == -1)
    {
        close(fd);
        T = (cell) counted_strerror();
        mu_throw();
    }
    p = (char *) mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED)
    {
        close(fd);
        T = (cell) counted_strerror();
        mu_throw();
    }

    NIPN(-1);
    SND = (cell) p;
    T = s.st_size;
}

/* NOTE: These two routines will be obsoleted by buf_* routines. */
void mu_read_carefully()
{
    int fd;
    char *buffer;
    size_t len;
    ssize_t count;

    fd = TRD;
    buffer = (char *) SND;
    len = T;
    DROP2;

    for (;;)
    {
        count = read(fd, buffer, len);
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

void mu_write_carefully()
{
    int fd;
    char *buffer;
    size_t len;
    ssize_t written;

    fd = TRD;
    buffer = (char *) SND;
    len = T;
    DROPN(3);

    while (len > 0)
    {
        written = write(fd, buffer, len);
        if (written == -1)
        {
            if (errno == EINTR) continue;
            PUSH(counted_strerror());
            mu_throw();
        }
        buffer += written;
        len -= written;
    }
}
