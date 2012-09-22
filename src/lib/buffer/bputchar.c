#include "buffer.h"

int
bputchar(Buffer *buf, uchar c)
{
	for (;;) {
		if (buf->nc <= buf->epb) {
			*buf->nc++ = c;
			return c;
		}

		if (buf->state == Dirty)
			if (bflush(buf) == EOF)
				return EOF;

		swtchbuf(buf);

		buf->state = Dirty;
		buf->nc = buf->bpb;
	}
}
