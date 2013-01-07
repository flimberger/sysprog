#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "defs.h"
#include "spc.h"

#include "class.h"
#include "error.h"

char *ext = "class";

void
gencode(char *outfile)
{
	char *nameend, *classname;
	Class *class;
	ptrdiff_t len;

	fprintf(stderr, "Generating class file...\n");
	nameend = strrchr(outfile, '.') + 1;
	if (strcmp(nameend, "class") != 0)
		die(1, "Output file must have a '.class' extension.");
	len = nameend - 1 - outfile;
	classname = calloc(len, sizeof(char));
	strncpy(classname, outfile, len);
	class = makeclass(out, classname);
	writeclass(class);
	/*
	gendecls(parsetree->left);
	genstmts(parsetree->right);
	*/
	freeclass(class);
	free(classname);
}
