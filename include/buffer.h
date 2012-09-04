/* buffering library */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stddef.h>

#include "defs.h"

enum {
	Blocksize	= 512,
	Bufsize		= 4 * 1024,
	Bungetsize	= 4,
	EOB		= -2,

	Readbuf		= 1 << 0,	/* status flags */
	Writebuf	= 1 << 1,

	Clean		= 1 << 8,
	Dirty		= 1 << 9
};

typedef struct {
	uchar *nc;	/* pointer to next character */
	uchar *bpb;	/* pointer to beginning of primary buffer */
	uchar *epb;	/* pointer to end of primary buffer */
	uchar *bsb;	/* pointer to beginning of secondary buffer */
	uchar *esb;	/* pointer to end of secondary buffer */
	void *mem;	/* pointer to (aligned) memory */
	size_t size;	/* size of buffers */
	int fd;		/* file descriptor */
	uint flags;	/* flags indicating state of buffer */
} Buffer;

Buffer *makebuf(size_t size); /* todo: args */
int initbuf(Buffer *buf, int fd, int mode); /* todo: args */
Buffer *openbuf(char *name, int mode);
int closebuf(Buffer *buf);
int termbuf(Buffer *buf);
void freebuf(Buffer *buf);
int bflush(Buffer *buf);

#endif /* _BUFFER_H_ */

