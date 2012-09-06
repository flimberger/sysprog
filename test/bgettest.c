#include <fcntl.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

size_t rdcnt = 1024;

int
main(int argc, char *argv[])
{
	int c;
	size_t i;
	Buffer *buf;

	if (argc < 2) {
		fprintf(stderr, "usage: %s file\n", argv[0]);
		exit(2);
	}

	errno = 0;
	if ((c = open(argv[1], O_RDONLY)) == -1) {
		fprintf(stderr, "%s: failed to open file %s: %s\n",
				argv[0], argv[1], strerror(errno));
		exit(1);
	}

	errno = 0;
	if ((buf = makebuf(512)) == NULL) {
		perror("allocating buffer failed:");
		exit(1);
	}

	errno = 0;
	if (initbuf(buf, c, O_RDONLY) == EOF) {
		perror("initializing buffer failed:");
		exit(1);
	}

	i = 0;
	errno = 0;
	while ((c = bgetchar(buf)) != EOF) {
		if (c)
			putc(c, stdout);
		errno = 0;
		i++;
	}
	if (errno != 0) {
		perror("an error occured:");
		exit(1);
	} else {
		fprintf(stderr, "%s: reached end of input file %s\n",
				argv[0], argv[1]);
		exit(0);
	}
	fprintf(stderr, "%s: %lu bytes read\n", argv[0], i);
	bclose(buf);

	exit(0);
}
