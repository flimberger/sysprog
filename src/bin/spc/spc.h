#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "buffer.h"
#include "defs.h"
#include "symtab.h"

typedef struct {
	union {
		long long val;
		Symbol *sym;
		const char *sign;
		char lastchar;
	} data;
	uint	row;
	uint	col;
	Symboltype	type;
} Token;

extern char *infile;
extern const char *const tokennames[];
extern Buffer *src, *out;
extern Symtab *symtab;

int compile(char *outfile);
Token *gettoken(void);
void parseprog(void);
void printtoken(Token *tp);
void syminit(void);
void symterm(void);

#endif /* _SYSPROG_H_ */
