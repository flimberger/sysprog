#include <fcntl.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "error.h"

enum {
	BUFFSIZE = 8
};

int
main(int argc, char *argv[])
{
	char	buf[BUFFSIZE];
	int	c, in, out, i, r, s;
	Buffer	*ibuf, *obuf;

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

	i = r = 0;
	while ((c = bgetchar(ibuf)) != EOF) {
		if (c) {
			if (r < BUFFSIZE - 1)
				buf[r++] = c;
			else {
				buf[r] = 0;
				fprintf(stderr, "log :%s\n", buf);
				if ((s = bprintf(obuf, "%s", buf)) != r) {
					fprintf(stderr, "written %d; requested %d\n", s, r);
					die(1, "bprintf failed:");
				}
				memset(buf, 0, BUFFSIZE);
				r = 0;
				buf[r++] = c;
			}
		}
		i++;
	}
	bprintf(obuf, "%s", buf);
	if (errno != 0)
		die(1, "bgetchar failed at char %d:", i);
	
	fprintf(stderr, "reached end of input file %s\n", argv[1]);
	fprintf(stderr, "%d bytes read\n", i);

	if (bclose(ibuf) == EOF)
		die(1, "closing input buffer failed:");

	if (bclose(obuf) == EOF)
		die(1, "closing output buffer failed:");

	exit(0);
}
