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

	if (argc < 2)
		die(2, "usage: %s file", argv[0]);

	if ((c = open(argv[1], O_RDONLY)) == -1)
		die(1, "%s: failed to open file %s:", argv[0], argv[1]);

	if ((buf = makebuf(512)) == NULL)
		die(1, "%s: allocating buffer failed:", argv[0]);

	if (initbuf(buf, c, O_RDONLY) == EOF)
		die(1, "%s: initializing buffer failed:", argv[0]);

	i = 0;
	while ((c = bgetchar(buf)) != EOF) {
		if (i == 511 || i == 512 || i == 513) {
			if (bungetchar(buf) == EOF) {
				die(1, "%s, bungetchar failed at char %lu", argv[0], i);
				break;
			}

			if ((c = bgetchar(buf)) == EOF)
				break;
		}
		putc(c, stdout);
		i++;
	}

	if (errno != 0)
		die(1, "%s: bgetchar failed at char %lu:", argv[0], i);

	fprintf(stderr, "%s: reached end of input file %s\n", argv[0], argv[1]);
	fprintf(stderr, "%s: %lu bytes read\n", argv[0], i);
	bclose(buf);

	exit(0);
}
