#include <stdio.h>

#include "buffer.h"

int
vbprintf(Buffer *buf, char *fmt, va_list args)
{
	uchar *bp;
	ptrdiff_t d;
	int l;

	if ((d = buf->epb - buf->nc + 1) < Bfmtstrlen) {
		if (buf->state == Dirty)
			if (bflush(buf) == EOF)
				return 0;

		bp = buf->bpb;
		buf->bpb = buf->bsb;
		buf->bsb = bp;

		/* end of secondary buf might be shorter because of previous buffer
		 * switch, so set the end pointer to buf begin + len */
		buf->epb = buf->bpb + buf->size - 1;
		/* the end of the primary buffer is the last char written to */
		buf->esb = buf->nc - 1;

		buf->nc = buf->bpb;

		buf->state = Dirty;
		d = Bufsize;
	}

	l = vsnprintf((char *) buf->nc, d, fmt, args);

	buf->nc += l;

	return l;
}
