#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "class.h"

#include "buffer.h"
#include "error.h"

static
void
writebyte(Buffer *buf, byte b)
{
	bputchar(buf, b);
}

static
void
writeword(Buffer *b, word w)
{
	writebyte(b, w >> 8);
	writebyte(b, w & 0xFF);
}

static
void
writedword(Buffer *b, dword d)
{
	writeword(b, d >> 16);
	writeword(b, d & 0xFFFF);
}

/*
static
void
writeqword(Buffer *b, qword q)
{
	writedword(b, q >> 32);
	writedword(b, q & 0xFFFFFFFF);
}
*/

static inline
Celem *
makeelem(Cpoolid id)
{
	Celem *e;

	if ((e = malloc(sizeof(Celem))) == NULL)
		die(2, "No memory:");
	e->id = id;
	return e;
}

static inline
void
insert(Class *c, Celem *e)
{
	e->next = c->cpool.list;
	c->cpool.list = e;
	c->cpool.size += 1;
}

void
cpaddarr(Class *c, Cpoolid id, const byte *const data, word size)
{
	Celem *e;

	e = makeelem(id);
	e->data.array.size = size;
	e->data.array.data = data;
	insert(c, e);
}

void cpaddtup(Class *c, Cpoolid id, word w, byte b)
{
	Celem *e;

	e = makeelem(id);
	e->data.tuple.w = w;
	e->data.tuple.b = b;
	insert(c, e);
}

void cpaddwords(Class *c, Cpoolid id, word a, word b)
{
	Celem *e;

	e = makeelem(id);
	e->data.words[0] = a;
	e->data.words[1] = b;
	insert(c, e);
}

void cpadddwords(Class *c, Cpoolid id, dword a, dword b)
{
	Celem *e;

	e = makeelem(id);
	e->data.dwords[0] = a;
	e->data.dwords[1] = b;
	insert(c, e);
}


Class *
makeclass(Buffer *file, const char *name)
{
	Class *c;

	c = malloc(sizeof(Class));
	c->file = file;
	c->vmin = JVM_VERSION_MINOR;
	c->vmaj = JVM_VERSION_MAJOR;
	c->cpool.size = 0;
	c->cpool.list = NULL;
	cpaddarr(c, Cpid_Utf8, (byte *) name, strlen(name));
	return c;
}

static
void
writehead(Class *c)
{
	writedword(c->file, 0xCAFEBABE);
	writeword(c->file, c->vmin);
	writeword(c->file, c->vmaj);
}

void
writeclass(Class *c)
{
	writehead(c);
}
