#include <fcntl.h>

#include <ctype.h>
/* #include <stdio.h> */
#include <stdlib.h>

#include "buffer.h"
#include "error.h"
#include "spc.h"
#include "strtab.h"
#include "symtab.h"

Buffer *src, *out;
char *pname;
Strtab *strtab;
Symbol **symtab;

int
main(int argc, char *argv[])
{
	char *infile, *outfile;

	pname = argv[0];
	if (argc < 2)
		die(1, "usage: %s OPTIONS file", pname);
	infile = argv[1];
	if (argc < 3)
		outfile = "sp.out";
	else
		outfile = argv[2];
	syminit();
	compile(infile, outfile);
	return 0;
}

int
compile(char *restrict infile, char *restrict outfile)
{
	Token *t;

	if ((src = bopen(infile, O_RDONLY)) == NULL)
		die(1, "%s: failed to allocate input buffer:", pname);
	if ((out = bopen(outfile, O_WRONLY)) == NULL)
		die(1, "%s: failed to allocate output buffer:", pname);
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
	if ((strtab = strtab_new()) == NULL)
		die(1, "%s: failed to allocate string table", pname);
	if ((symtab = makesymtab(SYMTABSIZE)) == NULL)
		die(1, "%s: failed to allocate symbol table", pname);
	atexit(symterm);
}

void
symterm(void)
{
/*
	size_t i, c[SYMTABSIZE];
	Symbol *s;

	for (i = 0; i < SYMTABSIZE; i++) {
		s = symtab[i];
		c[i] = 0;
		while (s != NULL) {
			c[i]++;
			s = s->next;
		}
		fprintf(stderr, "%lu\n", c[i]);
	}
*/
	freesymtab(symtab, 0);
	strtab_free(strtab);
}

void
printtoken(Token *tp)
{
	if (tp == NULL)
		return;
	switch (tp->type) {
	case END:
		bprintf(out, "End of file reached.");
		break;
	case ERROR:
		bprintf(out, "Token ERROR   at char %4d line %3d Char  %s\n", tp->col, tp->row, tp->data.sign);
	/*	bprintf(out, "Token ERROR   at char %4d line %3d\n",  tp->col, tp->row);
	*/
		break;
	case IDENTIFIER:
		bprintf(out, "Token IDENTIFIER char %4d line %3d Lexem %s\n", tp->col, tp->row, tp->data.sym->lexem);
		break;
	case INTEGER:
		bprintf(out, "Token INTEGER    char %4d line %3d Value %ld\n", tp->col, tp->row, tp->data.val);
		break;
	case PRINT:
		bprintf(out, "Token PRINT      char %4d line %3d Lexem %s\n", tp->col, tp->row, tp->data.sym);
		break;
	case READ:
		bprintf(out, "Token READ       char %4d line %3d Lexem %s\n", tp->col, tp->row, tp->data.sym);
		break;
	case SIGN:
		bprintf(out, "Token SIGN       char %4d line %3d Sign  %s\n", tp->col, tp->row, tp->data.sign);
		break;
	default:
		bprintf(out, "Unknown Token    char %4d line %3d Value %x\n", tp->col, tp->row, tp->type);
	}
}
