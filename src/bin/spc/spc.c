#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"
#include "spc.h"
#include "symtab.h"

#define STDOUTFILE	"sp.out"

Buffer *src, *out;
char *pname;
Symbol **symtab;

int
main(int argc, char *argv[])
{
	char *infile, *outfile;
	int i;

	infile = outfile = NULL;
	setpname(argv[0]);
	if (argc < 2)
		die(1, "usage: %s OPTIONS file", getpname());
	for (i = 1; i < argc; i++)
		if (argv[i][0] == '-') {
			if (argv[i][1] == 'o')
				outfile = argv[++i];
		} else
			infile = argv[i];
	if (outfile == NULL)
		outfile = STDOUTFILE;
	syminit();
	compile(infile, outfile);
	return 0;
}

int
compile(char *restrict infile, char *restrict outfile)
{
	Token *t;

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
		bprintf(out, "Token ERROR   at char %4d line %3d Char  %c\n", tp->col, tp->row, tp->data.lastchar);
		break;
	case IDENTIFIER:
		bprintf(out, "Token IDENTIFIER char %4d line %3d Lexem %s\n", tp->col, tp->row, tp->data.sym->lexem);
		break;
	case INTEGER:
		bprintf(out, "Token INTEGER    char %4d line %3d Value %ld\n", tp->col, tp->row, tp->data.val);
		break;
	case PRINT:
		bprintf(out, "Token PRINT      char %4d line %3d Lexem %s\n", tp->col, tp->row, tp->data.sym->lexem);
		break;
	case READ:
		bprintf(out, "Token READ       char %4d line %3d Lexem %s\n", tp->col, tp->row, tp->data.sym->lexem);
		break;
	case SIGN:
		bprintf(out, "Token SIGN       char %4d line %3d Sign  %s\n", tp->col, tp->row, tp->data.sign);
		break;
	default:
		bprintf(out, "Unknown Token    char %4d line %3d Value %x\n", tp->col, tp->row, tp->type);
	}
}
