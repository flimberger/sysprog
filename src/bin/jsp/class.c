#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "platform.h"
#include "class.h"

#include "buffer.h"
#include "error.h"
#include "opcodes.h"

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

void
addfield(Class *c, word acc, word nameidx, word descidx, word nattr, Attribute *lattr, bool fld)
{
	Field *f;

	if ((f = malloc(sizeof(Field))) == NULL)
		die(2, "No memory:");
	f->acc = acc;
	f->name = nameidx;
	f->desc = descidx;
	f->nattr = nattr;
	f->attrs = lattr;
	if (fld == true) {
		if (c->fields.size == 0)
			c->fields.list = f;
		else {
			f->next = c->fields.list;
			c->fields.list = f;
		}
		c->fields.size++;
	} else {
		if (c->methods.size == 0)
			c->methods.list = f;
		else {
			f->next = c->methods.list;
			c->methods.list = f;
		}
		c->methods.size++;
	}
}

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
	c->cpool.size++;
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
	Field *f;

	while (c->fields.list != NULL) {
		f = c->fields.list;
		c->fields.list = c->fields.list->next;
		free(f);
	}
	while (c->methods.list != NULL) {
		f = c->methods.list;
		c->methods.list = c->methods.list->next;
		free(f);
	}
	while (c->cpool.list != NULL) {
		e = c->cpool.list;
		c->cpool.list = c->cpool.list->next;
		free(e);
	}
	free(c);
}

Attribute *
makeattr(Attrid id, word nameidx)
{
	Attribute *a;

	if ((a = malloc(sizeof(Attribute))) == NULL)
		die(2, "No memory:");
	a->id = id;
	a->name = nameidx;
	return a;
}

Class *
makeclass(Buffer *file, const char *name)
{
	Attribute *a;
	Class *c;

	c = malloc(sizeof(Class));
	c->file = file;
	c->vmin = JVM_VERSION_MINOR;
	c->vmaj = JVM_VERSION_MAJOR;
	c->cpool.size = 0;
	c->cpool.list = NULL;
	c->acc = Acc_Super;
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
	/* Add constructor */
	cpaddarr(c, Cpid_Utf8, (byte *) STR_ATTR_CODE, strlen(STR_ATTR_CODE));
	a = makeattr(Attr_Code, c->cpool.size);
	a->info.code.maxstack = 1;
	a->info.code.maxlocals = 1;
	a->info.code.len = DEF_CONSTR_LEN;
	if ((a->info.code.code = calloc(DEF_CONSTR_LEN, sizeof(byte))) == NULL)
		die(2, "No memory:");
	a->info.code.code[0] = ALOAD_0;
	a->info.code.code[1] = INVOKESPECIAL;
	a->info.code.code[2] = (c->cpool.size - 1) >> 8;
	a->info.code.code[3] = (c->cpool.size - 1) & 0xFF;
	a->info.code.code[4] = RETURN;
	a->info.code.nexceptions = 0;
	a->info.code.nattr = 0;
	setattrsize(a);
	addfield(c, Acc_None, c->cpool.size - 4, c->cpool.size - 3, 1, a, false);
	/* Add main method (empty) */
	a = makeattr(Attr_Code, c->cpool.size);
	a->info.code.maxstack = 1;
	a->info.code.maxlocals = 1;
	a->info.code.len = 1;
	if ((a->info.code.code = calloc(1, sizeof(byte))) == NULL)
		die(2, "No memory:");
	a->info.code.code[0] = RETURN;
	a->info.code.nexceptions = 0;
	a->info.code.nattr = 0;
	setattrsize(a);
	cpaddarr(c, Cpid_Utf8, (byte *) STR_MAIN_NAME, strlen(STR_MAIN_NAME));
	cpaddarr(c, Cpid_Utf8, (byte *) STR_MAIN_TYPE, strlen(STR_MAIN_TYPE));
	addfield(c, Acc_Public | Acc_Static, c->cpool.size - 1, c->cpool.size, 1, a, false);
	return c;
}

void
setattrsize(Attribute *a)
{
	Attribute *al;
	dword s;

	s = 0;
	switch (a->id) {
	case Attr_Code:
		for (al = a->info.code.attrs; al != NULL; al = al->next)
			s += 6 + al->len;
		s += 12 + a->info.code.len + 8 * a->info.code.nexceptions;
		break;
	case Attr_Lntbl:
		break;
	}
	a->len = s;
}

static
void
writeattrs(Buffer *b, Attribute *alist)
{
	dword i;

	for ( ; alist != NULL; alist = alist->next) {
		fprintf(stderr, "\tWriting attribute:\n\tName index:%u\n", alist->name);
		writeword(b, alist->name);
		fprintf(stderr, "\tLength: %u\n", alist->len);
		writedword(b, alist->len);
		switch (alist->id) {
		case Attr_Code:
			fprintf(stderr, "\tCode:\n\t\tMax stack: %u\n", alist->info.code.maxstack);
			writeword(b, alist->info.code.maxstack);
			fprintf(stderr, "\t\tMax locals: %u\n", alist->info.code.maxlocals);
			writeword(b, alist->info.code.maxlocals);
			fprintf(stderr, "\t\tLength: %u\n", alist->info.code.len);
			writedword(b, alist->info.code.len);
			for (i = 0; i < alist->info.code.len; i++) {
				fprintf(stderr, "\t\t\t%04x: %02x\n", i, alist->info.code.code[i]);
				writebyte(b, alist->info.code.code[i]);
			}
			fprintf(stderr, "\t\tExceptions: %u elements\n", alist->info.code.nexceptions);
			writeword(b, alist->info.code.nexceptions);
			for (i = 0; i < alist->info.code.nexceptions; i++) {
				writeword(b, alist->info.code.exceptions[i].start);
				writeword(b, alist->info.code.exceptions[i].end);
				writeword(b, alist->info.code.exceptions[i].handler);
				writeword(b, alist->info.code.exceptions[i].catch);
			}
			fprintf(stderr, "\t\tAttributes: %u elements\n", alist->info.code.nattr);
			writeword(b, alist->info.code.nattr);
			writeattrs(b, alist->info.code.attrs);
			break;
		case Attr_Lntbl:
			break;
		}
	}
}

static
void
writefields(Buffer *b, Field *flist)
{
	for ( ; flist != NULL; flist = flist->next) {
		fprintf(stderr, "Writing field:\n\tAccess flags: %u\n", flist->acc);
		writeword(b, flist->acc);
		fprintf(stderr, "\tName index: %u\n", flist->name);
		writeword(b, flist->name);
		fprintf(stderr, "\tDescriptor index: %u\n", flist->desc);
		writeword(b, flist->desc);
		fprintf(stderr, "\tAttributes: %u elements\n", flist->nattr);
		writeword(b, flist->nattr);
		writeattrs(b, flist->attrs);
	}
}

void
writeclass(Class *c)
{
	Buffer *b;
	Celem *e;
	word i, cpi;

	b = c->file;
	writedword(b, 0xCAFEBABE);
	fprintf(stderr, "Java version: %u.%u\n", c->vmaj, c->vmin);
	writeword(b, c->vmin);
	writeword(b, c->vmaj);
	fprintf(stderr, "Constant pool: %u elements\n", c->cpool.size);
	writeword(b, c->cpool.size + 1);
	cpi = 0;
	for (e = c->cpool.list; e != NULL; e = e->next) {
		fprintf(stderr, "[%u]", cpi+++1);
		writebyte(b, e->id);
		switch (e->id) {
		case Cpid_Utf8:
			fprintf(stderr, "Cpid_Utf8: %s\n", (char *) e->data.array.data);
			writeword(b, e->data.array.size);
			for (i = 0; i < e->data.array.size; i++)
				writebyte(b, e->data.array.data[i]);
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
			fprintf(stderr, "Cpid_Class: name=%u\n", e->data.words[0]);
			writeword(b, e->data.words[0]);
			break;
		case Cpid_String:
			break;
		case Cpid_Fieldref:
			break;
		case Cpid_Methodref:
			fprintf(stderr, "Cpid_Methodref: class=%u; methodref=%u\n", e->data.words[0], e->data.words[1]);
			writeword(b, e->data.words[0]);
			writeword(b, e->data.words[1]);
			break;
		case Cpid_InterfaceMethodref:
			break;
		case Cpid_NameAndType:
			fprintf(stderr, "Cpid_NameAndType: name=%u; type=%u\n", e->data.words[0], e->data.words[1]);
			writeword(b, e->data.words[0]);
			writeword(b, e->data.words[1]);
			break;
		case Cpid_MethodHandle:
			break;
		case Cpid_MethodType:
			break;
		case Cpid_InvokeDynamic:
			break;
		}
	}
	fprintf(stderr, "Access flags: %04x\n", c->acc);
	writeword(b, c->acc);
	fprintf(stderr, "Class index: %u\n", c->this);
	writeword(b, c->this);
	fprintf(stderr, "Superclass index: %u\n", c->super);
	writeword(b, c->super);
	fprintf(stderr, "Interfaces: %u elements\n", c->interfaces.size);
	writeword(b, c->interfaces.size);
	fprintf(stderr, "Fields: %u elements\n", c->fields.size);
	writeword(b, c->fields.size);
	writefields(b, c->fields.list);
	fprintf(stderr, "Methods: %u elements\n", c->methods.size);
	writeword(b, c->methods.size);
	writefields(b, c->methods.list);
	fprintf(stderr, "Attributes: %u elements\n", c->attrs.size);
	writeword(b, c->attrs.size);
	writeattrs(b, c->attrs.list);
}
