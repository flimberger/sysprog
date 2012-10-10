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
	Symbol *sym, **symtab;

	if (argc > 1)
		fprintf(stderr, "%s: unused arguments\n", argv[0]);

	if ((symtab = makesymtab(0)) == NULL)
		die(1, "%s: failed to allocate symbol table:", argv[0]);

	if ((sym = storesym(symtab, "foo")) == NULL)
		die(1, "%s: failed to insert lexem \"foo\":", argv[0]);
	sym->info->type = IDENTIFIER;

	if ((sym = storesym(symtab, "bar")) == NULL)
		die(1, "%s: failed to insert lexem \"bar\":", argv[0]);
	sym->info->type = IDENTIFIER;

	if ((sym = storesym(symtab, "baz")) == NULL)
		die(1, "%s: failed to insert lexem \"baz\":", argv[0]);
	sym->info->type = IDENTIFIER;

	if ((sym = findsym(symtab, "foo")) == NULL)
		die(1, "%s: couldn't find lexem \"foo\":", argv[0]);
	if (sym->info->type != IDENTIFIER)
		die(1, "%s: type of \"foo\" is not IDENTIFIER", argv[0]);
	sym->info->type = INTEGER;

	if ((sym = findsym(symtab, "bar")) == NULL)
		die(1, "%s: couldn't find lexem \"bar\":", argv[0]);
	if (sym->info->type != IDENTIFIER)
		die(1, "%s: type of \"bar\" is not IDENTIFIER", argv[0]);
	sym->info->type = INTEGER;

	if ((sym = findsym(symtab, "baz")) == NULL)
		die(1, "%s: couldn't find lexem \"baz\":", argv[0]);
	if (sym->info->type != IDENTIFIER)
		die(1, "%s: type of \"bar\" is not IDENTIFIER", argv[0]);
	sym->info->type = INTEGER;

	if ((sym = findsym(symtab, "foo")) == NULL)
		die(1, "%s: couldn't find lexem \"foo\":", argv[0]);
	if (sym->info->type != INTEGER)
		die(1, "%s: type of \"foo\" is not INTEGER", argv[0]);

	if ((sym = findsym(symtab, "bar")) == NULL)
		die(1, "%s: couldn't find lexem \"bar\":", argv[0]);
	if (sym->info->type != INTEGER)
		die(1, "%s: type of \"bar\" is not INTEGER", argv[0]);

	if ((sym = findsym(symtab, "baz")) == NULL)
		die(1, "%s: couldn't find lexem \"baz\":", argv[0]);
	if (sym->info->type != INTEGER)
		die(1, "%s: type of \"bar\" is not INTEGER", argv[0]);

	freesymtab(symtab, 0);

	return 0;
}