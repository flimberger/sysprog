#include <fcntl.h>

#include <ctype.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"
#include "spc.h"
#include "strtab.h"
#include "symtab.h"

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
	Buffer *src, *out;

	if ((src = bopen(infile, O_RDONLY)) == NULL)
		die(1, "%s: failed to allocate input buffer:", pname);
	if ((out = bopen(outfile, O_WRONLY)) == NULL)
		die(1, "%s: failed to allocate output buffer:", pname);
	lex(src, out);
	bclose(out);
	bclose(src);
	return 0;
}

void
syminit(void)
{
	if ((strtab = strtab_new()) == NULL)
		die(1, "%s: failed to allocate string table", pname);
	if ((symtab = makesymtab(0)) == NULL)
		die(1, "%s: failed to allocate symbol table", pname);
	atexit(symterm);
}

void
symterm(void)
{
	freesymtab(symtab, 0);
	strtab_free(strtab);
}
