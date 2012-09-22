#include <fcntl.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "error.h"

int
main(int argc, char *argv[])
{
	int c, in, out;
	size_t i;
	Buffer *ibuf, *obuf;

	errno = 0;

	if (argc < 3)
		die(2, "usage: %s infile outfile", argv[0]);

	if ((in = open(argv[1], O_RDONLY)) == -1)
		die(1, "%s: failed to open file %s:", argv[0], argv[1]);

	if ((out = open(argv[2], O_WRONLY)) == -1)
		die(1, "%s: failed to open file %s:", argv[0], argv[2]);

	if ((ibuf = makebuf(512)) == NULL)
		die(1, "%s: allocating input buffer failed:", argv[0]);

	if ((obuf = makebuf(512)) == NULL)
		die(1, "%s: allocating output buffer failed!:", argv[0]);

	if (initbuf(ibuf, in, O_RDONLY) == EOF)
		die(1, "%s: initializing input buffer failed:", argv[0]);

	if (initbuf(obuf, out, O_WRONLY) == EOF)
		die(1, "%s: initializing output buffer failed:", argv[0]);

	i = 0;
	while ((c = bgetchar(ibuf)) != EOF) {
		if (c)
			if (bputchar(obuf, c) == EOF)
				die(1, "%s: bputchar failed:", argv[0]);
		i++;
	}
	if (errno != 0)
		die(1, "%s: bgetchar failed at char %lu:", argv[0], i);
	
	fprintf(stderr, "%s: reached end of input file %s\n", argv[0], argv[1]);
	fprintf(stderr, "%s: %lu bytes read\n", argv[0], i);

	if (bclose(ibuf) == EOF)
		die(1, "%s: closing input buffer failed:", argv[0]);

	if (bclose(obuf) == EOF)
		die(1, "%s: closing output buffer failed:", argv[0]);

	exit(0);
}
