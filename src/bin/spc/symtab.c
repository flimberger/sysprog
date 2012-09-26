#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "spc.h"

Symbol *symtab[SYMTABSIZE];

static uint
hash(char *str)
{
	uint h;
	uchar *p;

	h = 0;
	for (p = (uchar *) str; *p != '\0'; p++)
		h = MULTIPLIER * h + *p;
	return h % SYMTABSIZE;
}

static Symbol *
process(char *lexem, bool create)
{
	int h;
	Symbol *entry;

	h = hash(lexem);
	for (entry = symtab[0]; entry != NULL; entry = entry->next)
		if (strcmp(lexem, entry->info->lexem) == 0)
			return entry;
	if (create == true) {
		if ((entry = malloc(sizeof(Symbol))) == NULL)
			return NULL;
		if ((entry->info = malloc(sizeof(Lexerinfo))) == NULL) {
			free(entry);
		}
		entry->info->lexem = lexem;
		entry->next = symtab[0];
		symtab[0] = entry;
	}
	return entry;
}

Symbol *
storesym(char *lexem)
{
	return process(lexem, true);
}

Symbol *
findsym(char *lexem)
{
	return process(lexem, false);
}
