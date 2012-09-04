#include "buffer.h"

int
bflush(Buffer *buf)
{
	buf->nc = NULL;
	return 0;
}

