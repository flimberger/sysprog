#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"
#include "spc.h"
#include "symtab.h"

#define STDOUTFILE	"sp.out"

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
	Token *t;

	if (outfile == NULL)
		outfile = STDOUTFILE;
	if ((src = bopen(infile, O_RDONLY)) == NULL)
		die(1, "failed to allocate input buffer:");
	if ((out = bopen(outfile, O_WRONLY)) == NULL)
		die(1, "failed to allocate output buffer:");
	for (t = gettoken(); t->type != ERROR && t->type != END; t = gettoken())
		printtoken(t);
	printtoken(t);
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
	s = storesym(symtab, "print");
	s->type = PRINT;
	s = storesym(symtab, "read");
	s->type = READ;
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
	switch (tp->type) {
	case END:
		break;
	case ERROR:
		bprintf(out, "Token %-10s char %4d line %3d Char  %c\n", tokennames[tp->type], tp->col, tp->row, tp->data.lastchar);
		break;
	case INTEGER:
		bprintf(out, "Token %-10s char %4d line %3d Value %ld\n", tokennames[tp->type], tp->col, tp->row, tp->data.val);
		break;
	case IDENTIFIER:
	case PRINT:
	case READ:
		bprintf(out, "Token %-10s char %4d line %3d Lexem %s\n", tokennames[tp->type], tp->col, tp->row, tp->data.sym->lexem);
		break;
	case SIGN_PLUS:
	case SIGN_MINUS:
	case SIGN_DIV:
	case SIGN_MULT:
	case SIGN_LESS:
	case SIGN_GRTR:
	case SIGN_EQUAL:
	case SIGN_UNEQL:
	case SIGN_NOT:
	case SIGN_AND:
	case SIGN_TERM:
	case SIGN_PAROP:
	case SIGN_PARCL:
	case SIGN_CBOP:
	case SIGN_CBCL:
	case SIGN_BROP:
	case SIGN_BRCL:
		bprintf(out, "Token %-10s char %4d line %3d Sign  %s\n", tokennames[tp->type], tp->col, tp->row, tp->data.sign);
		break;
	default:
		bprintf(out, "Unknown Token    char %4d line %3d Value %x\n", tp->col, tp->row, tp->type);
	}
}
