/* our version of buffered i/o */

#include "muforth.h"

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

struct buf
{
    int fd;
    char *buffer;
    size_t capacity;
    size_t behind;
    size_t ahead;
};

ssize_t buf_refill(struct buf *pbuf)
{
    ssize_t count;

    for (;;)
    {
	count = read(pbuf->fd, pbuf->buffer, pbuf->capacity);
	if (count == -1)
	{
	    if (errno == EINTR) continue;
	    PUSH(strerror);
	    throw();
	}
	break;
    }
    pbuf->behind = 0;
    pbuf->ahead = count;
}

ssize_t buf_read(struct buf *pbuf, char *data, size_t len)
{
    if (pbuf->ahead == 0) buf_refill(pbuf);
    if (len > pbuf->ahead) len = pbuf->ahead; /* only get what we have */
    memcpy(data, pbuf->buffer + pbuf->behind, len);
    pbuf->behind += len;
    pbuf->ahead -= len;
    return len;
}

static void write_all(int fd, char *buffer, size_t len)
{
    ssize_t written;

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

void buf_flush(struct buf *pbuf)
{
    if (pbuf->behind == 0) return;
    write_all(pbuf->fd, pbuf->buffer, pbuf->behind);
    pbuf->behind = 0;
    pbuf->ahead = pbuf->capacity;
}

static ssize_t buf_write_once(struct buf *pbuf, char *data, size_t len)
{
    if (len > pbuf->ahead) len = pbuf->ahead; /* fill the buffer */	
    memcpy(pbuf->buffer + pbuf->behind, data, len);
    pbuf->behind += len;
    pbuf->ahead -= len;
    if (pbuf->ahead == 0) buf_flush(pbuf);
    return len;
}


void buf_write(struct buf *pbuf, char *data, size_t len)
{
    ssize_t written;

    while (len > 0)
    {
	written = buf_write_once(pbuf, data, len);
	data += written;
	len -= written;
    }
}

