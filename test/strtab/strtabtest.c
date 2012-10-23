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

	setpname(argv[0]);
	if (argc > 1)
		fprintf(stderr, "%s: unused arguments\n", getpname());

	if ((tab = makestrtab()) == NULL)
		die(1, "allocating strtab failed:");

	foo = storestr(tab, "foo");
	bar = storestr(tab, "bar");
	bat = storestr(tab, "bat");
	mf = storestr(tab, "motherfucking");
	baz = storestr(tab, "baz");

	f = 0;
	if (strcmp(foo, "foo") != 0) {
		f++;
		fprintf(stderr, "%d. failure: pointer to \"foo\" failed\n", f);
	}

	if (strcmp(bar, "bar") != 0) {
		f++;
		fprintf(stderr, "%d. failure: pointer to \"bar\" failed\n", f);
	}

	if (strcmp(bat, "bat") != 0) {
		f++;
		fprintf(stderr, "%d. failure: pointer to \"bat\" failed\n", f);
	}

	if (strcmp(mf, "motherfucking") != 0) {
		f++;
		fprintf(stderr, "%d. failure: pointer to \"motherfucking\" failed\n", f);
	}

	if (strcmp(baz, "baz") != 0) {
		f++;
		fprintf(stderr, "%d. failure: pointer to \"baz\" failed\n", f);
	}

	if (f != 0)
		fprintf(stderr, "%d pointer comparisions failed.\n", f);
	else
		fprintf(stderr, "all pointer comparisons successful\n");

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

	freestrtab(tab);

	return 0;
}