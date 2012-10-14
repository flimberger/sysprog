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

#define PERM (S_IRUSR | S_IWUSR | S_IRGRP)

Buffer *
bopen(char *name, int mode)
{
	Buffer *bp;

	if ((bp = makebuf(Bufsize)) == NULL)
		return NULL; /* errno set by makebuf() */
	switch (mode) {
	case O_RDONLY:
		if ((bp->fd = open(name, O_RDONLY | O_DIRECT)) == -1) {
			freebuf(bp);
			return NULL; /* errno is set by open() */
		}
		break;
	case O_WRONLY:
		if ((bp->fd = creat(name, PERM)) == -1) {
			freebuf(bp);
			return NULL; /* errno is set by open() */
		}
		break;
	default:
		errno = EINVAL;
		return NULL;
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
	int fd;

	fd = buf->fd;
	if (termbuf(buf) == EOF)
		return EOF; /* TODO: error handling! */
	if (close(fd) == -1)
		return EOF; /* errno is set by close() */
	freebuf(buf);

	return 0;
}

