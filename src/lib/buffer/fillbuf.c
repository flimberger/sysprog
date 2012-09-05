#include <unistd.h>

#include "buffer.h"

int
fillbuf(Buffer *buf)
{
	buf->flags |= Clean;
	return 0;
}

