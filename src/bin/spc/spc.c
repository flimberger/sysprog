#include <fcntl.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "error.h"
#include "spc.h"
#include "symtab.h"

const char *const tokennames[] = {
	"S_NONE",
	"S_ELSE",
	"S_END",
	"S_ERROR",
	"S_IDENT",
	"S_IF",
	"S_INT",
	"S_INTCONST",
	"S_PRINT",
	"S_READ",
	"S_WHILE",
	"S_PLUS",
	"S_MINUS",
	"S_DIV",
	"S_MULT",
	"S_LESS",
	"S_GRTR",
	"S_EQUAL",
	"S_UNEQL",
	"S_NOT",
	"S_AND",
	"S_TERM",
	"S_PAROP",
	"S_PARCL",
	"S_CBOP",
	"S_CBCL",
	"S_BROP",
	"S_BRCL"
};

char *infile;
Buffer *src, *out;
Symtab *symtab;
Node *parsetree;

int
main(int argc, char *argv[])
{
	char *outfile;
	int i;

	setpname(argv[0]);
	syminit();
	infile = outfile = NULL;
	if (argc < 2)
		die(1, "usage: %s OPTIONS file", getpname());
	for (i = 1; i < argc; i++)
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'o')
				outfile = argv[++i];
			else
				die(1, "unknown argument %s", argv[i]);
		} else
			infile = argv[i];
	compile(outfile);
	return 0;
}

int
compile(char *outfile)
{
	char *b, *e;
	ptrdiff_t l;

	if (outfile == NULL) {
		b = strrchr(infile, '/') + 1;
		if (b == (char *) 1)
			b = infile;
		e = strrchr(infile, '.') + 1;
		l = e - b;
		outfile = calloc(l + 3, sizeof(char));
		strncpy(outfile, b, l);
		strncat(outfile, "out", 3);
	}
	if ((src = bopen(infile, O_RDONLY)) == NULL)
		die(1, "failed to allocate input buffer:");
	if ((out = bopen(outfile, O_WRONLY)) == NULL)
		die(1, "failed to allocate output buffer:");
	parseprog();
	checkprog(parsetree);
	genprog(parsetree);
	bclose(out);
	bclose(src);
	return 0;
}

void
syminit(void)
{
	Symbol *s;

	if ((symtab = makesymtab(SYMTABSIZE)) == NULL)
		die(1, "failed to allocate symbol table");
	s = storesym(symtab, "else");
	s->symtype = S_ELSE;
	s = storesym(symtab, "if");
	s->symtype = S_IF;
	s = storesym(symtab, "int");
	s->symtype = S_INT;
	s = storesym(symtab, "print");
	s->symtype = S_PRINT;
	s = storesym(symtab, "read");
	s->symtype = S_READ;
	s = storesym(symtab, "while");
	s->symtype = S_WHILE;
	atexit(symterm);
}

void
symterm(void)
{
	freesymtab(symtab);
}
