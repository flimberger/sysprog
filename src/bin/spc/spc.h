#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "defs.h"

enum {
	MULTIPLIER = 31,
	SYMTABSIZE = 32
};

typedef struct Symbol Symbol;

typedef enum {
	sign,
	integer,
	identifier,
	print,
	read
} Symboltype;

typedef struct {
	char *lexem;
	long value;
	int line;
	int column;
	Symboltype type;
} Lexerinfo;

struct Symbol {
	Symbol *next;
	Lexerinfo *info;
};

Symbol *storesym(char *lexem);
Symbol *findsym(char *lexem);

#endif /* _SYSPROG_H_ */
