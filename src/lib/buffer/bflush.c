#include <unistd.h>

#include "buffer.h"

int
bbflush(Buffer *buf)
{
	if (bflush(buf) == EOF)
		return EOF;

	if (buf->nc > buf->bpb) {
		buf->epb = buf->nc - 1;
		buf->state = Dirty;
	}

	swtchbuf(buf);

	return bflush(buf);
}

int
bflush(Buffer *buf)
{
	ptrdiff_t d;

	if (buf->state == Clean)
		return 0;

	if (buf->flags & Writebuf) {
		d = buf->esb - buf->bsb + 1;
		if (write(buf->fd, buf->bsb, d) != d) {
			buf->flags &= ~Active;
			buf->nc = buf->bsb;
			return EOF; /* errno set by write() */
		}
		buf->state = Clean;
	}

	return 0;
}
