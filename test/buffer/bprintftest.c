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

	i = r = 0;
	while ((c = bgetchar(ibuf)) != EOF) {
		if (c) {
			if (r < BUFFSIZE)
				buf[r++] = c;
			else {
				buf[r] = 0;
				fprintf(stderr, "log :%s\n", buf);
				if ((s = bprintf(obuf, "%s", buf)) != r) {
					fprintf(stderr, "written %d; requested %d\n", s, r);
					die(1, "%s: bprintf failed:", argv[0]);
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
		die(1, "%s: bgetchar failed at char %d:", argv[0], i);
	
	fprintf(stderr, "%s: reached end of input file %s\n", argv[0], argv[1]);
	fprintf(stderr, "%s: %d bytes read\n", argv[0], i);

	if (bclose(ibuf) == EOF)
		die(1, "%s: closing input buffer failed:", argv[0]);

	if (bclose(obuf) == EOF)
		die(1, "%s: closing output buffer failed:", argv[0]);

	exit(0);
}
