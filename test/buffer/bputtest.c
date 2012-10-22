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

	setpname(argv[0]);
	if (argc < 3)
		die(2, "usage: %s infile outfile", getpname());

	if ((in = open(argv[1], O_RDONLY)) == -1)
		die(1, "failed to open file %s:", argv[1]);

	if ((out = open(argv[2], O_WRONLY)) == -1)
		die(1, "failed to open file %s:", argv[2]);

	if ((ibuf = makebuf(512)) == NULL)
		die(1, "allocating input buffer failed:");

	if ((obuf = makebuf(512)) == NULL)
		die(1, "allocating output buffer failed!:");

	if (initbuf(ibuf, in, O_RDONLY) == EOF)
		die(1, "initializing input buffer failed:");

	if (initbuf(obuf, out, O_WRONLY) == EOF)
		die(1, "initializing output buffer failed:");

	i = 0;
	while ((c = bgetchar(ibuf)) != EOF) {
		if (c)
			if (bputchar(obuf, c) == EOF)
				die(1, "bputchar failed:");
		i++;
	}
	if (errno != 0)
		die(1, "bgetchar failed at char %lu:", i);
	
	fprintf(stderr, "reached end of input file %s\n", argv[1]);
	fprintf(stderr, "%lu bytes read\n", i);

	if (bclose(ibuf) == EOF)
		die(1, "closing input buffer failed:");

	if (bclose(obuf) == EOF)
		die(1, "closing output buffer failed:");

	exit(0);
}
