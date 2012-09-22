/* buffering library */

#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdarg.h>
#include <stddef.h>

#include "defs.h"

#ifndef EOF
#define EOF (-1)
#endif

enum {
	Blocksize	= 512,
	Bufsize		= 4 * 1024,
	Bungetsize	= 4,
	NBuf		= 8,		/* # of buffers/process */

	Readbuf		= 1 << 0,	/* status flags positions */
	Writebuf	= 1 << 1,
	Active		= 1 << 2,	/* active if set */

	Dirty		= 0,		/* state of second buffer */
	Clean
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
	int state;	/* state of second buffer: Clean or Dirty */
	uint flags;	/* flags indicating state of buffer */
} Buffer;

static inline void
swtchbuf(Buffer *buf)
{
	uchar *bp;

	bp = buf->bpb;
	buf->bpb = buf->bsb;
	buf->bsb = bp;

	bp = buf->epb;
	buf->epb = buf->esb;
	buf->esb = bp;
}

Buffer *makebuf(size_t size);
int initbuf(Buffer *buf, int fd, int mode);
Buffer *bopen(char *name, int mode);
int bclose(Buffer *buf);
int termbuf(Buffer *buf);
void freebuf(Buffer *buf);
int bflush(Buffer *buf);
int bbflush(Buffer *buf);
int bgetchar(Buffer *buf);
int bungetchar(Buffer *buf);
int fillbuf(Buffer *buf);
int bputchar(Buffer *buf, uchar c);
int bprintf(Buffer *buf, char *fmt, ...);
int vbprintf(Buffer *buf, char *fmt, va_list args);

#endif /* _BUFFER_H_ */
