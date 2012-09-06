#include <string.h>

#include "buffer.h"

static inline void
swtch(Buffer *buf)
{
	uchar *bp;

	bp = buf->bpb;
	buf->bpb = buf->bsb;
	buf->bsb = bp;

	bp = buf->epb;
	buf->epb = buf->esb;
	buf->esb = bp;
}

int
bgetchar(Buffer *buf)
{
	int c;
	ptrdiff_t d;

getchar:
	if (buf->nc <= buf->epb) {
		if (buf->nc > buf->bpb + Bungetsize)
			buf->state = Clean;
		c = *buf->nc++;
		return c;
	}

	if ((buf->flags & Active) == 0)
		return EOF;

	if (buf->nc > buf->epb) {
		if (buf->state == Clean)
			if (fillbuf(buf) == EOF)
				return EOF; /* errno is set by read() */

		if ((d = buf->esb - buf->bsb) <= Bungetsize) {
			memcpy(buf->bpb, buf->epb - Bungetsize, Bungetsize);
			memcpy(buf->bpb + Bungetsize, buf->bsb, d + 1);
			buf->epb = buf->bpb + Bungetsize + d;
			buf->nc = buf->bpb + Bungetsize;
			buf->state = Clean;
		} else {
			swtch(buf);
			buf->nc = buf->bpb;
		}

		goto getchar;
	}

	buf->nc = buf->bsb;
	buf->flags &= ~Active;
	return EOF;
}

int
bungetchar(Buffer *buf)
{
	if (buf->nc > buf->bpb) {
		buf->nc--;
		return 0;
	}

	if (buf->state == Dirty) {
		swtch(buf);
		buf->nc = buf->epb;
		return 0;
	}

	return EOF;
}

