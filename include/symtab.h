#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include <stddef.h>

#include "defs.h"
#include "strtab.h"

enum {
	SYMTABSIZE = 4096
};

typedef struct Symbol Symbol;
struct Symbol {
	Symbol *next;
	const char *lexem;
	Symboltype symtype;
	Datatype   datatype;
};

typedef struct {
	Symbol **symbols;
	Strtab *strtab;
	size_t size;
} Symtab;

Symtab *makesymtab(size_t size);
void freesymtab(Symtab *tab);
Symbol *storesym(Symtab *restrict tab, const char *restrict const lexem);
Symbol *findsym(Symtab *restrict tab, const char *restrict const lexem);

#endif /* _SYMTAB_H_ */
