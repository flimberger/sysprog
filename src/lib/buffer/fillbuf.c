#include <unistd.h>

#include "buffer.h"

int
fillbuf(Buffer *buf)
{
	ssize_t i;

	if (buf->state == Dirty)
		return EOF;	/* TODO: errno? */

	if ((i = read(buf->fd, buf->bsb, buf->size)) <= 0)
		return EOF;	/* errno set by read(), if an error occured */

	buf->esb = buf->bsb + i - 1;
	buf->state = Dirty;
	return 0;
}

