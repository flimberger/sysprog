#include <stdio.h>
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
	e->next = NULL;
	return e;
}

static inline
void
insert(Class *c, Celem *e)
{
	Celem *end;
	if (c->cpool.list == NULL)
		c->cpool.list = e;
	else {
		for (end = c->cpool.list; end->next != NULL; end = end->next)
			;
		end->next = e;
	}
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

void
freeclass(Class *c)
{
	Celem *e;
	
	while (c->cpool.list != NULL) {
		e = c->cpool.list;
		free(e);
		c->cpool.list = c->cpool.list->next;
	}
	free(c);
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
	/* Add class to constant pool */
	cpaddarr(c, Cpid_Utf8, (byte *) name, strlen(name));
	cpaddwords(c, Cpid_Class, c->cpool.size, 0);
	c->this = c->cpool.size;
	/* Add super class to constant pool */
	cpaddarr(c, Cpid_Utf8, (byte *) STR_JAVA_OBJECT, strlen(STR_JAVA_OBJECT));
	cpaddwords(c, Cpid_Class, c->cpool.size, 0);
	c->super = c->cpool.size;
	c->interfaces.size = 0;
	c->fields.size = 0;
	c->methods.size = 0;
	c->attrs.size = 0;
	/* Add superclass constructor */
	cpaddarr(c, Cpid_Utf8, (byte *) STR_CONSTR_NAME, strlen(STR_CONSTR_NAME));
	cpaddarr(c, Cpid_Utf8, (byte *) STR_CONSTR_TYPE, strlen(STR_CONSTR_TYPE));
	cpaddwords(c, Cpid_NameAndType, c->cpool.size - 1, c->cpool.size);
	cpaddwords(c, Cpid_Methodref, c->super, c->cpool.size);
	return c;
}

void
writeclass(Class *c)
{
	Celem *e;
	word i;

	writedword(c->file, 0xCAFEBABE);
	writeword(c->file, c->vmin);
	writeword(c->file, c->vmaj);
	writeword(c->file, c->cpool.size + 1);
	fprintf(stderr, "%u\n", c->cpool.size);
	e = c->cpool.list;
	while (e != NULL) {
		writebyte(c->file, e->id);
		switch (e->id) {
		case Cpid_Utf8:
			fprintf(stderr, "writing Cpid_Utf8: %s\n", (char *) e->data.array.data);
			writeword(c->file, e->data.array.size);
			for (i = 0; i < e->data.array.size; i++)
				writebyte(c->file, e->data.array.data[i]);
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
			fprintf(stderr, "writing Cpid_Class: name=%u\n", e->data.words[0]);
			writeword(c->file, e->data.words[0]);
			break;
		case Cpid_String:
			break;
		case Cpid_Fieldref:
			break;
		case Cpid_Methodref:
			fprintf(stderr, "writing Cpid_InterfaceMethodref: class=%u; methodref=%u\n", e->data.words[0], e->data.words[0]);
			writeword(c->file, e->data.words[0]);
			writeword(c->file, e->data.words[1]);
			break;
		case Cpid_InterfaceMethodref:
			break;
		case Cpid_NameAndType:
			fprintf(stderr, "writing Cpid_NameAndType: name=%u; type=%u\n", e->data.words[0], e->data.words[0]);
			writeword(c->file, e->data.words[0]);
			writeword(c->file, e->data.words[1]);
			break;
		case Cpid_MethodHandle:
			break;
		case Cpid_MethodType:
			break;
		case Cpid_InvokeDynamic:
			break;
		}
		e = e->next;
	}
	writeword(c->file, c->accflags);
	writeword(c->file, c->this);
	writeword(c->file, c->super);
	writeword(c->file, c->interfaces.size);
	writeword(c->file, c->fields.size);
	writeword(c->file, c->methods.size);
	writeword(c->file, c->attrs.size);
}
