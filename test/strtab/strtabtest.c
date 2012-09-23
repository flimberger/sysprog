#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "error.h"
#include "strtab.h"

int
main(int argc, char *argv[])
{
	uint f;
	const char *foo, *bar, *mf, *baz;
	size_t i;
	strtab_elem *tab, *p;

	if (argc > 1)
		fprintf(stderr, "%s: unused arguments\n", argv[0]);

	if ((tab = strtab_new(8)) == NULL)
		die(1, "%s: allocating strtab failed:", argv[0]);

	fprintf(stderr, "%p\n", (void *) tab);

	foo = strtab_insert(tab, "foo");
	bar = strtab_insert(tab, "bar");
	mf = strtab_insert(tab, "motherfucking");
	baz = strtab_insert(tab, "baz");

	fprintf(stderr, "%p\n", (void *) tab);

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

	p = tab;
	while (p->nextelem != NULL) {
		for (i = 0; i < p->size; ++i) {
			if (p->data[i] == '\0')
				putchar('\n');
			else
				putchar(p->data[i]);
		}
		p = p->nextelem;
	}

	strtab_free(tab);

	return 0;
}