#include <stdio.h>

#include "platform.h"

#include "defs.h"
#include "spc.h"
#include "jvm.h"

#include "buffer.h"
#include "error.h"

char *ext = "class";

static
void
writebyte(byte b)
{
	bputchar(out, b);
}

static
void
writeword(word w)
{
	writebyte(w >> 8);
	writebyte(w & 0xFF);
}

static
void
writedword(dword d)
{
	writeword(d >> 16);
	writeword(d & 0xFFFF);
}

/*
static
void
writeqword(qword q)
{
	writedword(q >> 32);
	writedword(q & 0xFFFFFFFF);
}
*/

static
void
writehead(word vmaj, word vmin)
{
	writedword(0xCAFEBABE);
	writeword(vmin);
	writeword(vmaj);
}

void
gencode(void)
{
	fprintf(stderr, "Generating class file...\n");
	writehead(JVM_VERSION_MAJOR, JVM_VERSION_MINOR);
	/*
	gendecls(parsetree->left);
	genstmts(parsetree->right);
	*/
}
