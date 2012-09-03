#include "buffer.h"

int
flushbuf(Buffer *buf)
{
	buf->nc = NULL;
	return 0;
}

