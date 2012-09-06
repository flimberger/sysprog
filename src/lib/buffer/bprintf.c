#include "buffer.h"

int
bprintf(Buffer *buf, char *fmt, ...)
{
	va_list args;
	int r;

	va_start(args, fmt);
	r = vbprintf(buf, fmt, args);
	va_end(args);

	return r;
}

