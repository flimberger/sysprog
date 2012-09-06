/* ugly, unportable linux hack */
#ifndef _GNU_SOURCE
#error O_DIRECT is not available on you system
#endif

#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include <errno.h>

#include "buffer.h"

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
bclose(Buffer *buf)
{
	if (bflush(buf) == EOF)
		return EOF; /* TODO: error handling! */
	if (close(buf->fd) == -1)
		return EOF; /* errno is set by close() */
	freebuf(buf);

	return 0;
}

