#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "error.h"
#include "symtab.h"

/*
 * This test needs a special patched version of the stringtab library, where
 * STRTAB_SIZE is 8 and the strtab_elem struct has a size_t size element.
 */
int
main(int argc, char *argv[])
{
	Symtab *symtab;
	Symbol *sym;

	setpname(argv[0]);
	if (argc > 1)
		fprintf(stderr, "%s: unused arguments\n", getpname());

	if ((symtab = makesymtab(0)) == NULL)
		die(1, "failed to allocate symbol table:");

	if ((sym = storesym(symtab, "foo")) == NULL)
		die(1, "failed to insert lexem \"foo\":");
	sym->type = IDENTIFIER;

	if ((sym = storesym(symtab, "bar")) == NULL)
		die(1, "failed to insert lexem \"bar\":");
	sym->type = IDENTIFIER;

	if ((sym = storesym(symtab, "baz")) == NULL)
		die(1, "failed to insert lexem \"baz\":");
	sym->type = IDENTIFIER;

	if ((sym = findsym(symtab, "foo")) == NULL)
		die(1, "couldn't find lexem \"foo\":");
	if (sym->type != IDENTIFIER)
		die(1, "type of \"foo\" is not IDENTIFIER");
	sym->type = INTEGER;

	if ((sym = findsym(symtab, "bar")) == NULL)
		die(1, "couldn't find lexem \"bar\":");
	if (sym->type != IDENTIFIER)
		die(1, "%s: type of \"bar\" is not IDENTIFIER");
	sym->type = INTEGER;

	if ((sym = findsym(symtab, "baz")) == NULL)
		die(1, "couldn't find lexem \"baz\":");
	if (sym->type != IDENTIFIER)
		die(1, "type of \"baz\" is not IDENTIFIER");
	sym->type = INTEGER;

	if ((sym = findsym(symtab, "foo")) == NULL)
		die(1, "couldn't find lexem \"foo\":");
	if (sym->type != INTEGER)
		die(1, "type of \"foo\" is not INTEGER");

	if ((sym = findsym(symtab, "bar")) == NULL)
		die(1, "couldn't find lexem \"bar\":");
	if (sym->type != INTEGER)
		die(1, "type of \"bar\" is not INTEGER");

	if ((sym = findsym(symtab, "baz")) == NULL)
		die(1, "couldn't find lexem \"baz\":");
	if (sym->type != INTEGER)
		die(1, "type of \"baz\" is not INTEGER");

	freesymtab(symtab);

	return 0;
}
