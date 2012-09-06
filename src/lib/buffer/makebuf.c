#if ! (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600)
#error posix_memalign() is not available at your system
#endif

/*
 * TODO: ugly linux hack, O_DIRECT is available on non-GNU (=sane?) systems too
 */
#ifndef _GNU_SOURCE
#error O_DIRECT is not available at your system
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include <errno.h>
#include <stdlib.h>

#include "defs.h"
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
			bflush(bp);
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
	/* TODO: append etc? */
	switch (mode) {
	case O_RDONLY:
		buf->flags = Readbuf;
		break;
	case O_WRONLY:
		buf->flags = Writebuf;
		if (registerbuf(buf) == EOF)
			return EOF;
		break;
	default:
		errno = EINVAL;
		return EOF;
	}

	buf->bpb = buf->mem;
	buf->epb = buf->bpb + buf->size - 1;
	buf->bsb = buf->bpb + buf->size;
	buf->esb = buf->bsb + buf->size - 1;
	buf->nc = buf->bsb;
	buf->flags |= Active;
	buf->state = Clean;
	buf->fd = fd;

	return 0;
}

Buffer *
bopen(char *name, int mode)
{
	Buffer *bp;
	int oflags;

	/* TODO: append etc? */
	switch (mode) {
	case O_RDONLY:
	case O_WRONLY:
		oflags = mode | O_DIRECT;
		break;
	default:
		errno = EINVAL;
		return NULL;
	}

	if ((bp = makebuf(Bufsize)) == NULL)
		return NULL; /* errno set by makebuf() */

	/* TODO: file creation if no file exists, and opened for writing */
	if ((bp->fd = open(name, oflags)) == -1) {
		freebuf(bp);
		return NULL; /* errno is set by open() */
	}

	if (initbuf(bp, bp->fd, mode) == EOF) {
		freebuf(bp);
		close(bp->fd);
		return NULL; /* errno is set by initbuf() */
	}

	return bp;
}

int
termbuf(Buffer *buf)
{
	int r;

	r = bflush(buf);
	if (buf->flags & Writebuf)
		unregisterbuf(buf);
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

int
bclose(Buffer *buf)
{
	if (bflush(buf) == EOF)
		return EOF; /* TODO: error handling! */
	if (close(buf->fd) == -1)
		return EOF; /* errno is set by close() */
	freebuf(buf);

	return 0;
}

