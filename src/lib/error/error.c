#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

static inline
void
vwarn(const char *fmt, va_list al)
{
	fflush(NULL);
	vfprintf(stderr, fmt, al);
	if (fmt[0] != '\0' && fmt[strlen(fmt) - 1] == ':')
		fprintf(stderr, " %s", strerror(errno));
	fprintf(stderr, "\n");
}

void
warn(const char *fmt, ...)
{
	va_list al;

	va_start(al, fmt);
	vwarn(fmt, al);
	va_end(al);
}

void
die(int status, const char *fmt, ...)
{
	va_list al;

	va_start(al, fmt);
	vwarn(fmt, al);
	va_end(al);

	exit(status);
}

void
panic(const char *fmt, ...)
{
	va_list al;

	va_start(al, fmt);
	vwarn(fmt, al);
	va_end(al);

	abort();
}

