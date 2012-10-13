#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "buffer.h"
#include "defs.h"
#include "symtab.h"

typedef struct {
	union {
		long long val;
		Symbol	*sym;
		char	*sign;
	} data;
	uint	row;
	uint	col;
	Symboltype	type;
} Token;

int compile(char *restrict infile, char *restrict outfile);
void lex(Buffer *restrict ibuf, Buffer *restrict obuf);
void syminit(void);
void symterm(void);

#endif /* _SYSPROG_H_ */
