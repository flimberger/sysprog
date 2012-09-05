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
		c = *buf->nc++;
		return c;
	}

	if (buf->nc > buf->epb) {
		if ((buf->flags & Clean) != 0)
			if (fillbuf(buf) == EOF)
				return EOF;

		if ((d = buf->esb - buf->bsb) <= Bungetsize) {
			memcpy(buf->bpb, buf->epb - Bungetsize, Bungetsize);
			memcpy(buf->bpb + Bungetsize, buf->bsb, d);
			buf->epb = buf->bpb + Bungetsize + d;
			buf->flags |= Clean;
		} else {
			swtch(buf);
		}

		buf->nc = buf->bpb;
		goto getchar;
	}

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

	if ((buf->flags & Clean) == 0) {
		swtch(buf);
		buf->nc = buf->epb;
		return 0;
	}

	return EOF;
}

