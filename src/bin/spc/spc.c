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
	ptrdiff_t nl;
	size_t el;

	if (outfile == NULL) {
		b = strrchr(infile, '/') + 1;
		e = strrchr(infile, '.') + 1;
		nl = e - b;
		el = strlen(ext);
		outfile = calloc(nl + el, sizeof(char));
		strncpy(outfile, b, nl);
		strncat(outfile, ext, el);
	}
	if ((src = bopen(infile, O_RDONLY)) == NULL)
		die(1, "failed to allocate input buffer:");
	if ((out = bopen(outfile, O_WRONLY)) == NULL)
		die(1, "failed to allocate output buffer:");
	parseprog();
	gencode();
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

void
printtoken(Token *tp)
{
	if (tp == NULL)
		return;
	switch (tp->symtype) {
	case S_NONE:
		fprintf(stderr, "Token %-10s char %4d line %3d Char  %c\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.lastchar);
		break;
	case S_END:
		break;
	case S_ERROR:
		fprintf(stderr, "Token %-10s char %4d line %3d Char  %c\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.lastchar);
		break;
	case S_INTCONST:
		bprintf(out, "Token %-10s char %4d line %3d Value %lld\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.val);
		break;
	case S_ELSE:
	case S_IF:
	case S_INT:
	case S_IDENT:
	case S_PRINT:
	case S_READ:
	case S_WHILE:
		bprintf(out, "Token %-10s char %4d line %3d Lexem %s\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.sym->lexem);
		break;
	case S_PLUS:
	case S_MINUS:
	case S_DIV:
	case S_MULT:
	case S_LESS:
	case S_GRTR:
	case S_EQUAL:
	case S_UNEQL:
	case S_NOT:
	case S_AND:
	case S_TERM:
	case S_PAROP:
	case S_PARCL:
	case S_CBOP:
	case S_CBCL:
	case S_BROP:
	case S_BRCL:
		bprintf(out, "Token %-10s char %4d line %3d Sign  %s\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.sign);
		break;
	default:
		die(3, "Unknown Token %s on char %4d line %3d\n", tokennames[tp->symtype], tp->col, tp->row);
	}
}
