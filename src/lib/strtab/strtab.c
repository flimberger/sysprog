#include <stdlib.h>
#include <string.h>

#include "strtab.h"

static
struct strtab_elem *
newelem(size_t size)
{
	struct strtab_elem *p;

	if ((p = malloc(sizeof(struct strtab_elem))) == NULL)
		return NULL;
	if ((p->data = calloc(size, sizeof(char))) == NULL) {
		free(p);
		return NULL;
	}
	return p;
}

static
void
freeelem(struct strtab_elem *list)
{
	free(list->data);
	free(list);
}

static
struct strtab_elem *
addfront(struct strtab_elem *restrict list, struct strtab_elem *restrict elem)
{
	elem->next = list;
	return elem;
}

static
struct strtab_elem *
addend(struct strtab_elem *restrict list, struct strtab_elem *restrict elem)
{
	struct strtab_elem *p;

	if (list == NULL)
		return elem;
	for (p = list; p->next != NULL; p = p->next)
		;
	p->next = elem;
	return list;
}

static
void
freeall(struct strtab_elem *list)
{
	struct strtab_elem *p;

	while (list != NULL) {
		p = list->next;
		freeelem(list);
		list = p;
	}
}

Strtab *
makestrtab(void)
{
	Strtab *tab;

	if ((tab = malloc(sizeof(Strtab))) == NULL)
		return NULL;
	if ((tab->list = newelem(STRTAB_SIZE)) == NULL) {
		free(tab);
		return NULL;
	}
	tab->fptr = tab->list->data;
	tab->fsiz = STRTAB_SIZE;
	return tab;
}

void
freestrtab(Strtab *tab)
{
	freeall(tab->list);
	free(tab);
}

const char *
storestr(Strtab *restrict tab, const char *const restrict str)
{
	struct strtab_elem *p;
	size_t sl;
	char *rs;

	sl = strlen(str) + 1;
	if (sl > tab->fsiz) {
		if (sl > STRTAB_SIZE) {
			if ((p = newelem(sl)) == NULL)
				return NULL;
			tab->list = addend(tab->list, p);
			return strncpy(p->data, str, sl);
		}
		else {
			if ((p = newelem(STRTAB_SIZE)) == NULL)
				return NULL;
			tab->list = addfront(tab->list, p);
			tab->fptr = tab->list->data;
			tab->fsiz = STRTAB_SIZE;
		}
	}
	rs = strncpy(tab->fptr, str, sl);
	tab->fptr += sl;
	tab->fsiz -= sl;
	return rs;
}
