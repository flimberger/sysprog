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
	c->accflags = 0;
	cpaddarr(c, Cpid_Utf8, (byte *) name, strlen(name));
	cpaddwords(c, Cpid_Class, c->cpool.size - 1, 0);
	c->this = c->cpool.size - 1;
	cpaddarr(c, Cpid_Utf8, (byte *) STR_JAVA_OBJECT, strlen(STR_JAVA_OBJECT));
	cpaddwords(c, Cpid_Class, c->cpool.size - 1, 0);
	c->super = c->cpool.size - 1;
	c->interfaces.size = 0;
	c->fields.size = 0;
	c->methods.size = 0;
	c->attrs.size = 0;
	return c;
}

void
writeclass(Class *c)
{
	Celem *e;
	word i, j;

	writedword(c->file, 0xCAFEBABE);
	writeword(c->file, c->vmin);
	writeword(c->file, c->vmaj);
	writeword(c->file, c->cpool.size);
	for (i = 0; i < c->cpool.size; i++) {
		e = c->cpool.list;
		writebyte(c->file, e->id);
		switch (e->id) {
		case Cpid_Utf8:
			writeword(c->file, e->data.array.size);
			for (j = 0; j < e->data.array.size; j++)
				writebyte(c->file, e->data.array.data[j]);
			break;
		case Cpid_Integer:
			break;
		case Cpid_Float:
			break;
		case Cpid_Long:
			break;
		case Cpid_Double:
			break;
		case Cpid_Class:
			break;
		case Cpid_String:
			break;
		case Cpid_Fieldref:
			break;
		case Cpid_Methodref:
			break;
		case Cpid_InterfaceMethodref:
			break;
		case Cpid_NameAndType:
			break;
		case Cpid_MethodHandle:
			break;
		case Cpid_MethodType:
			break;
		case Cpid_InvokeDynamic:
			break;
		}
	}
}
