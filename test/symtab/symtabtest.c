#include <stdio.h>
#include <string.h>

#include "defs.h"
#include "error.h"
#include "symtab.h"

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
	sym->symtype = S_IDENT;

	if ((sym = storesym(symtab, "bar")) == NULL)
		die(1, "failed to insert lexem \"bar\":");
	sym->symtype = S_IDENT;

	if ((sym = storesym(symtab, "baz")) == NULL)
		die(1, "failed to insert lexem \"baz\":");
	sym->symtype = S_IDENT;

	if ((sym = findsym(symtab, "foo")) == NULL)
		die(1, "couldn't find lexem \"foo\":");
	if (sym->symtype != S_IDENT)
		die(1, "symtype of \"foo\" is not S_IDENT");
	sym->symtype = S_INTCONST;

	if ((sym = findsym(symtab, "bar")) == NULL)
		die(1, "couldn't find lexem \"bar\":");
	if (sym->symtype != S_IDENT)
		die(1, "%s: symtype of \"bar\" is not S_IDENT");
	sym->symtype = S_INTCONST;

	if ((sym = findsym(symtab, "baz")) == NULL)
		die(1, "couldn't find lexem \"baz\":");
	if (sym->symtype != S_IDENT)
		die(1, "symtype of \"baz\" is not S_IDENT");
	sym->symtype = S_INTCONST;

	if ((sym = findsym(symtab, "foo")) == NULL)
		die(1, "couldn't find lexem \"foo\":");
	if (sym->symtype != S_INTCONST)
		die(1, "symtype of \"foo\" is not S_INTCONST");

	if ((sym = findsym(symtab, "bar")) == NULL)
		die(1, "couldn't find lexem \"bar\":");
	if (sym->symtype != S_INTCONST)
		die(1, "symtype of \"bar\" is not S_INTCONST");

	if ((sym = findsym(symtab, "baz")) == NULL)
		die(1, "couldn't find lexem \"baz\":");
	if (sym->symtype != S_INTCONST)
		die(1, "symtype of \"baz\" is not S_INTCONST");

	freesymtab(symtab);

	return 0;
}
