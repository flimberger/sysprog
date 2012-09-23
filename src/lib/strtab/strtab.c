#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strtab.h"

strtab_elem *
strtab_new(size_t size)
{
	strtab_elem *tab;

	if (size == 0)
		size = STRTAB_SIZE;

	if ((tab = malloc(sizeof(strtab_elem))) == NULL)
		return NULL;
	if ((tab->data = calloc(sizeof(char), size)) == NULL) {
		free(tab);
		return NULL;
	}
	tab->nextelem = NULL;
	tab->nextstr = tab->data;
	tab->size = size;

	return tab;
}

void
strtab_free(strtab_elem *tab)
{
	strtab_elem *p;

	while (tab->nextelem != NULL) {
		p = tab->nextelem;
		free(tab);
		tab = p;
	}
}

const char *
strtab_insert(strtab_elem *restrict tab, const char *const restrict str)
{
	strtab_elem *p;
	ssize_t sl;
	char *rs;

	sl = strlen(str) + 1;
	if (sl > ((tab->data + tab->size) - tab->nextstr)) {
		fprintf(stderr, "creating new strtab_elem with size %ld\n", sl);
		if (sl > STRTAB_SIZE) {
			p = strtab_new(sl);
		}
		else {
			p = strtab_new(STRTAB_SIZE);
		}
		if (p == NULL)
			return NULL;
		fprintf(stderr, "new elem: %p; old elem: %p\n", (void *) p, (void *) tab);
		p->nextelem = tab;
		tab = p;
		fprintf(stderr, "new elem: %p; old elem: %p\n", (void *) p, (void *) tab);
	}
	rs = strncpy(tab->nextstr, str, sl);
	tab->nextstr += sl;

	return rs;
}
