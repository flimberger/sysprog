#include <errno.h>
#include <string.h>

#include "buffer.h"

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

	if (buf->state == Clean) {
		errno = 0;
		if (fillbuf(buf) == EOF) {
			if (errno != 0) {
				buf->nc = buf->bsb;
				buf->flags &= ~Active;
			}
			return EOF;
		}
	}

	if ((d = buf->esb - buf->bsb) <= Bungetsize) {
		memcpy(buf->bpb, buf->epb - Bungetsize, Bungetsize);
		memcpy(buf->bpb + Bungetsize, buf->bsb, d + 1);
		buf->epb = buf->bpb + Bungetsize + d;
		buf->nc = buf->bpb + Bungetsize;
		buf->state = Clean;
	} else {
		swtchbuf(buf);
		buf->nc = buf->bpb;
	}

	goto getchar;
}

int
bungetchar(Buffer *buf)
{
	if (buf->nc > buf->bpb) {
		buf->nc--;
		return 0;
	}

	if (buf->state == Dirty) {
		swtchbuf(buf);
		buf->nc = buf->epb;
		return 0;
	}

	return EOF;
}
