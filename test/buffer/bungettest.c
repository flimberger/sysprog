#include <fcntl.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "error.h"

size_t rdcnt = 1024;

int
main(int argc, char *argv[])
{
	int c;
	size_t i;
	Buffer *buf;

	errno = 0;

	setpname(argv[0]);
	if (argc < 2)
		die(2, "usage: %s file", getpname());

	if ((c = open(argv[1], O_RDONLY)) == -1)
		die(1, "failed to open file %s:", argv[1]);

	if ((buf = makebuf(512)) == NULL)
		die(1, "allocating buffer failed:");

	if (initbuf(buf, c, O_RDONLY) == EOF)
		die(1, "initializing buffer failed:");

	i = 0;
	while ((c = bgetchar(buf)) != EOF) {
		if (i == 511 || i == 512 || i == 513) {
			if (bungetchar(buf) == EOF) {
				die(1, "bungetchar failed at char %lu", i);
				break;
			}

			if ((c = bgetchar(buf)) == EOF)
				break;
		}
		putc(c, stdout);
		i++;
	}

	if (errno != 0)
		die(1, "bgetchar failed at char %lu:", i);

	fprintf(stderr, "reached end of input file %s\n", argv[1]);
	fprintf(stderr, "%lu bytes read\n", i);
	bclose(buf);

	exit(0);
}
