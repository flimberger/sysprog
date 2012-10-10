#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stddef.h>

#include "defs.h"

enum {
	SYMTABSIZE = 32
};

typedef struct Symbol Symbol;
struct Symbol {
	Symbol *next;
	Lexerinfo *info;
};

Symbol **makesymtab(size_t size);
void freesymtab(Symbol **tab, size_t size);
Symbol *storesym(Symbol **restrict tab, const char *restrict const lexem);
Symbol *findsym(Symbol **restrict tab, const char *restrict const lexem);

#endif /* _SYMTAB_H_ */
