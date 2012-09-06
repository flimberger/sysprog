#include "buffer.h"

int
bputchar(Buffer *buf, uchar c)
{
	for (;;) {
		if (buf->nc <= buf->epb) {
			*buf->nc++ = c;
			return c;
		}

		if (bflush(buf) == EOF)
			return EOF;
	}
}

