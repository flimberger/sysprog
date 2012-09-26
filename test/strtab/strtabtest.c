#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "error.h"
#include "strtab.h"

/*
 * This test needs a special patched version of the stringtab library, where
 * STRTAB_SIZE is 8 and the strtab_elem struct has a size_t size element.
 */
int
main(int argc, char *argv[])
{
	uint f,l;
	const char *foo, *bar, *bat, *mf, *baz;
	size_t i;
	Strtab *tab;
	struct strtab_elem *p;

	if (argc > 1)
		fprintf(stderr, "%s: unused arguments\n", argv[0]);

	if ((tab = strtab_new()) == NULL)
		die(1, "%s: allocating strtab failed:", argv[0]);

	foo = strtab_insert(tab, "foo");
	bar = strtab_insert(tab, "bar");
	bat = strtab_insert(tab, "bat");
	mf = strtab_insert(tab, "motherfucking");
	baz = strtab_insert(tab, "baz");

	f = 0;
	if (strcmp(foo, "foo") != 0) {
		f++;
		fprintf(stderr, "%s: %d. failure: pointer to \"foo\" failed\n", argv[0], f);
	}

	if (strcmp(bar, "bar") != 0) {
		f++;
		fprintf(stderr, "%s: %d. failure: pointer to \"bar\" failed\n", argv[0], f);
	}

	if (strcmp(mf, "motherfucking") != 0) {
		f++;
		fprintf(stderr, "%s: %d. failure: pointer to \"motherfucking\" failed\n", argv[0], f);
	}

	if (strcmp(baz, "baz") != 0) {
		f++;
		fprintf(stderr, "%s: %d. failure: pointer to \"baz\" failed\n", argv[0], f);
	}

	if (f != 0)
		fprintf(stderr, "%s: %d pointer comparisions failed.\n", argv[0], f);
	else
		fprintf(stderr, "%s: all pointer comparisons successful\n", argv[0]);

	p = tab->list;
	l = 1;
	while (p != NULL) {
		printf("%d. elem:\n", l);
		for (i = 0; i < p->size; ++i) {
			if (p->data[i] == '\0')
				putchar('.');
			else
				putchar(p->data[i]);
		}
		p = p->next;
		l++;
		putchar('\n');
	}

	strtab_free(tab);

	return 0;
}