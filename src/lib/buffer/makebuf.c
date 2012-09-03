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

Buffer *
makebuf(size_t size)
{
	int r;
	Buffer *bp;

	if ((bp = (Buffer *) malloc(sizeof(Buffer))) == NULL)
		return NULL; /* errno set by malloc() */

	if ((r = posix_memalign((void **) &bp->mem, Blocksize, size * 2)) != 0) {
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
	/* TODO: state handling */
	switch (mode) {
	case O_RDONLY:
		break;
	case O_WRONLY:
		break;
	default:
		errno = EINVAL;
		return EOB;
	}

	buf->nc = buf->bpb;
	buf->bpb = buf->mem;
	buf->epb = buf->bpb + buf->size - 1;
	buf->bsb = buf->bpb + buf->size;
	buf->esb = buf->bsb + buf->size;
	buf->fd = fd;

	return 0;
}

Buffer *
openbuf(char *name, int mode)
{
	Buffer *bp;
	int fd, oflags;

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

	/* TODO: get rid of annoying gcc warning for uninitialized fd */
	fd = 0;

	/* TODO: file creation if no file exists */
	if ((bp->fd = open(name, oflags)) == -1) {
		freebuf(bp);
		return NULL; /* errno is set by open() */
	}

	if (initbuf(bp, fd, mode) == EOB) {
		freebuf(bp);
		close(fd);
		return NULL; /* errno is set by initbuf() */
	}

	return bp;
}

int
termbuf(Buffer *buf)
{
	int r;

	r = flushbuf(buf);
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
closebuf(Buffer *buf)
{
	int r;

	if ((r = flushbuf(buf)) == EOB)
		return r; /* TODO: error handling! */
	if ((r = close(buf->fd)) == -1)
		return r; /* errno is set by close() */
	freebuf(buf);

	return 0;
}

