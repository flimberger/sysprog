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
process(Symbol **restrict tab, const char *restrict const lexem, bool create)
{
	uint h;
	Symbol *entry;

	h = hash(lexem);
	for (entry = tab[h]; entry != NULL; entry = entry->next)
		if (strcmp(lexem, entry->info->lexem) == 0)
			return entry;
	if (create == true) {
		if ((entry = malloc(sizeof(Symbol))) == NULL)
			return NULL;
		if ((entry->info = malloc(sizeof(Lexerinfo))) == NULL) {
			free(entry);
			return NULL;
		}
		entry->info->lexem = lexem;
		entry->next = tab[h];
		tab[h] = entry;
	}
	return entry;
}

Symbol *
storesym(Symbol **restrict tab, const char *restrict const lexem)
{
	return process(tab, lexem, true);
}

Symbol *
findsym(Symbol **restrict tab, const char *restrict const lexem)
{
	return process(tab, lexem, false);
}

Symbol **
makesymtab(size_t size)
{
	if (size == 0)
		size = SYMTABSIZE;
	return calloc(size, sizeof(Symbol *));
}

void
freesymtab(Symbol **tab, size_t size)
{
	size_t i;
	Symbol *entry, *next;

	if (size == 0)
		size = SYMTABSIZE;
	for (i = 0; i < size; i++) {
		for (entry = tab[i]; entry != NULL; entry = next) {
			next = entry->next;
			free(entry->info);
			free(entry);
		}
	}
	free(tab);
}
