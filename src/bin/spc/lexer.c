#include "buffer.h"
#include "error.h"
#include "spc.h"

int
main(int argc, char *argv[])
{
	Buffer *b;

	if (argc < 2)
		die(1, "usage: %s OPTIONS file", argv[0]);
	if ((b = bopen(argv[1], 1)) == NULL)
		die(1, "%s: failed to allocate input buffer:");
	bclose(b);
	return 0;
}
