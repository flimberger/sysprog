#include <unistd.h>

#include "buffer.h"

int
bbflush(Buffer *buf)
{
	if (bflush(buf) == EOF)
		return EOF;

	if (buf->nc > buf->bpb)
		buf->state = Dirty;

	swtchbuf(buf);

	return bflush(buf);
}

int
bflush(Buffer *buf)
{
	ptrdiff_t d;
	ssize_t i;

	if (buf->state == Clean)
		return 0;

	if (buf->flags & Writebuf) {
		d = buf->esb - buf->bsb;
		for (i = 0; i < Bufsize; i += Blocksize)
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

	return 0;
}
