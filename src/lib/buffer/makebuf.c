#if ! (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
#error posix_memalign() is not available at your system
#endif

#include <fcntl.h>

#include <errno.h>
#include <stdlib.h>

#include "buffer.h"

static Buffer	*writereg[NBuf];
static int 	registered;

static void
flushregister(void)
{
	Buffer *bp;
	int i;

	for (i = 0; i < NBuf; ++i) {
		bp = writereg[i];
		if (bp != 0) {
			bbflush(bp);
			writereg[i] = 0;
		}
	}
}

static void
unregisterbuf(Buffer *buf)
{
	int i;

	for (i = 0; i < NBuf; ++i)
		if (writereg[i] == buf)
			writereg[i] = 0;
}

static int
registerbuf(Buffer *buf)
{
	int i;

	unregisterbuf(buf);
	for (i = 0; i < NBuf; ++i)
		if (writereg[i] == 0) {
			writereg[i] = buf;
			if (registered == 0) {
				registered = 1;
				atexit(flushregister);
			}
			return 0;
		}

	errno = EMFILE;
	return EOF;
}

Buffer *
makebuf(size_t size)
{
	int r;
	Buffer *bp;

	if ((bp = (Buffer *) malloc(sizeof(Buffer))) == NULL)
		return NULL; /* errno set by malloc() */

	if ((r = posix_memalign(&bp->mem, Blocksize, size * 2)) != 0) {
		free(bp);
		errno = r;
		return NULL;
	}

	bp->size = size;
	bp->fd = -1;
	bp->flags = 0;

	return bp;
}

int
initbuf(Buffer *buf, int fd, int mode)
{

	buf->bpb = buf->mem;
	buf->epb = buf->bpb + buf->size - 1;
	buf->bsb = buf->bpb + buf->size;
	buf->esb = buf->bsb + buf->size - 1;
	buf->state = Clean;
	buf->fd = fd;

	/* TODO: append etc? */
	switch (mode) {
	case O_RDONLY:
		buf->flags = Readbuf;
		buf->nc = buf->bsb;
		break;
	case O_WRONLY:
		if (registerbuf(buf) == EOF)
			return EOF;
		buf->flags = Writebuf;
		buf->nc = buf->bpb;
		break;
	default:
		errno = EINVAL;
		return EOF;
	}

	buf->flags |= Active;

	return 0;
}

int
termbuf(Buffer *buf)
{
	int r;

	r = 0;
	if (buf->flags & Writebuf) {
		r = bbflush(buf);
		unregisterbuf(buf);
	}
	buf->nc = NULL;
	buf->fd = -1;
	buf->flags = 0;

	return r;
}

void
freebuf(Buffer *buf)
{
	free(buf->mem);
	free(buf);
	buf = NULL;
}
