/* file primitives */

#include "muforth.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

/* XXX: for read, write; temporary? */
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

void open_file()		/* C-string-name flags - fd */
{
    int fd;

    fd = open((char *) STK(1), TOP);
    if (fd == -1)
    {
	TOP = (int) "couldn't open";
	throw();
    }
    STK(1) = fd;
    DROP(1);
}

void push_ro_flags()
{
    PUSH(O_RDONLY);
}

void push_rw_flags()
{
    PUSH(O_RDWR);
}

void push_create_flags()
{
    STK(-1) = O_CREAT | O_TRUNC | O_WRONLY;
    STK(-2) = 0666;
    DROP(-2);
}

void close_file()
{
    for (;;)
    {
	if(close(TOP) == -1)
	{
	    if (errno == EINTR) continue;
	    TOP = (int) strerror(errno);
	    throw();
	}
	break;
    }
    DROP(1);
}

void mmap_file()		/* fd - addr len */
{
    char *p;
    struct stat s;
    int fd;

    fd = TOP;

    if (fstat(fd, &s) == -1)
    {
	close(fd);
	TOP = (int) strerror(errno);
	throw();
    }
    p = (char *) mmap(0, s.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED)
    {
	close(fd);
	TOP = (int) strerror(errno);
	throw();
    }

    TOP = (int) p;
    STK(-1) = s.st_size;
    DROP(-1);
    /* PUSH(p); */
    /* PUSH(s.st_size); */
}

void load_file()
{
    int fd;

    PUSH(O_RDONLY);
    open_file();
    fd = TOP;
    mmap_file();
    PUSH(evaluate);
    catch();
    close(fd);
    throw();
}

/* NOTE: These two routines will be obsoleted by buf_* routines. */
void read_carefully()
{
    int fd;
    char *buffer;
    size_t len;
    ssize_t count;

    fd = STK(2);
    buffer = (char *) STK(1);
    len = TOP;
    DROP(2);

    for (;;)
    {
	count = read(fd, buffer, len);
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

void write_carefully()
{
    int fd;
    char *buffer;
    size_t len;
    ssize_t written;

    fd = STK(2);
    buffer = (char *) STK(1);
    len = TOP;
    DROP(3);

    while (len > 0)
    {
	written = write(fd, buffer, len);
	if (written == -1)
	{
	    if (errno == EINTR) continue;
	    PUSH(strerror);
	    throw();
	}
	buffer += written;
	len -= written;
    }
}
