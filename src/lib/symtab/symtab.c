#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "symtab.h"

enum {
	MULTIPLIER = 31
};

static uint
hash(const char *str)
{
	uint h;
	uchar *p;

	h = 0;
	for (p = (uchar *) str; *p != '\0'; p++)
		h = MULTIPLIER * h + *p;
	return h % SYMTABSIZE;
}

static Symbol *
process(Symtab *restrict tab, const char *restrict const lexem, bool create)
{
	uint h;
	Symbol *entry;

	entry = NULL;
	h = hash(lexem);
	for (entry = tab->symbols[h]; entry != NULL; entry = entry->next)
		if (strcmp(lexem, entry->lexem) == 0)
			return entry;
	if (create == true) {
		if ((entry = malloc(sizeof(Symbol))) == NULL)
			return NULL;
		if ((entry->lexem = storestr(tab->strtab, lexem)) == NULL) {
			free(entry);
			return NULL;
		}
		entry->next = tab->symbols[h];
		entry->type = 0;
		tab->symbols[h] = entry;

	}
	return entry;
}

Symbol *
storesym(Symtab *restrict tab, const char *restrict const lexem)
{
	return process(tab, lexem, true);
}

Symbol *
findsym(Symtab *restrict tab, const char *restrict const lexem)
{
	return process(tab, lexem, false);
}

Symtab *
makesymtab(size_t size)
{
	Symtab *symtab;
	if (size == 0)
		size = SYMTABSIZE;
	if ((symtab = malloc(sizeof(Symtab))) != NULL) {
		symtab->size = size;
		if ((symtab->symbols = calloc(size, sizeof(Symbol *))) == NULL) {
			free(symtab);
			return NULL;
		}
		if ((symtab->strtab = makestrtab()) == NULL) {
			free(symtab->symbols);
			free(symtab);
			return NULL;
		}
	}
	return symtab;
}

void
freesymtab(Symtab *tab)
{
	size_t i;
	Symbol *entry, *next;

	for (i = 0; i < tab->size; i++) {
		for (entry = tab->symbols[i]; entry != NULL; entry = next) {
			next = entry->next;
			free(entry);
		}
	}
	free(tab->symbols);
	freestrtab(tab->strtab);
	free(tab);
}
