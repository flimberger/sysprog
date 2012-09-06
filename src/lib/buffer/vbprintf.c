#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"

int
vbprintf(Buffer *buf, char *fmt, va_list args)
{
	uchar *bp;
	ptrdiff_t d;
	int l;

	if ((d = buf->epb - buf->nc) < 512) {
		if (buf->state == Dirty)
			if (bflush(buf) == EOF)
				return EOF;

		bp = buf->bpb;
		buf->bpb = buf->bsb;
		buf->bsb = bp;

		bp = buf->epb;
		buf->epb = buf->bsb + buf->size - 1;
		buf->esb = buf->epb;
	}

	l = vsprintf((char *) buf->nc, fmt, args);

	if (l > d) {
		fprintf(stderr, "vbprintf: memory fault");
		exit(1);
	}

	buf->nc += l;

	return l;
}

