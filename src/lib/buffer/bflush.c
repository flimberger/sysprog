#include <unistd.h>

#include "buffer.h"

int
bflush(Buffer *buf)
{
	ptrdiff_t d;
	ssize_t i;

	if (buf->state == Clean)
		return 0;

	if (buf->flags & Readbuf) {
		d = buf->nc - buf->esb;
		for (i = 0; i < Bufsize / Blocksize; i += Blocksize) {
			if (i > d) {
				if (write(buf->fd, buf->bsb, i) != i) {
					buf->flags &= ~Active;
					buf->nc = buf->bsb;
					return EOF; /* errno set by write() */
				}
				buf->state = Clean;
				return 0;
			}
		}
	}

	return 0;
}

