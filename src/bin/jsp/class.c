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
word
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
	return ++c->cpool.size;
}

word
cpaddarr(Class *c, Cpoolid id, const byte *const data, word size)
{
	Celem *e;

	e = makeelem(id);
	e->data.array.size = size;
	e->data.array.data = data;
	return insert(c, e);
}

word
cpaddtup(Class *c, Cpoolid id, word w, byte b)
{
	Celem *e;

	e = makeelem(id);
	e->data.tuple.w = w;
	e->data.tuple.b = b;
	return insert(c, e);
}

word
cpaddwords(Class *c, Cpoolid id, word a, word b)
{
	Celem *e;

	e = makeelem(id);
	e->data.words[0] = a;
	e->data.words[1] = b;
	return insert(c, e);
}

word
cpadddwords(Class *c, Cpoolid id, dword a, dword b)
{
	Celem *e;

	e = makeelem(id);
	e->data.dwords[0] = a;
	e->data.dwords[1] = b;
	return insert(c, e);
}

word
getprint(Class *c)
{
	Attribute *a;
	word name, desc, fldref, mtdref, i, n, d;

	if (c->print == 0) {
		/* method this.print */
		name = cpaddarr(c, Cpid_Utf8, (byte *) STR_PRNT_NAME, strlen(STR_PRNT_NAME));
		desc = cpaddarr(c, Cpid_Utf8, (byte *) STR_PRNT_DESC, strlen(STR_PRNT_DESC));
		i = cpaddwords(c, Cpid_NameAndType, name, desc);
		c->print = cpaddwords(c, Cpid_Methodref, c->this, i);
		/* field System.out */
		n = cpaddarr(c, Cpid_Utf8, (byte *) STR_OUT_NAME, strlen(STR_OUT_NAME));
		d = cpaddarr(c, Cpid_Utf8, (byte *) STR_OUT_DESC, strlen(STR_OUT_DESC));
		i = cpaddwords(c, Cpid_NameAndType, n, d);
		fldref = cpaddwords(c, Cpid_Fieldref, getsys(c), i);
		/* class PrintStream*/
		n = cpaddarr(c, Cpid_Utf8, (byte *) STR_PRNTSTRM_NAME, strlen(STR_PRNTSTRM_NAME));
		i = cpaddwords(c, Cpid_Class, n, 0);
		/* method PrintStream.println, has the same descriptor as this.print */
		n = cpaddarr(c, Cpid_Utf8, (byte *) STR_PRNTLN_NAME, strlen(STR_PRNTLN_NAME));
		d = cpaddwords(c, Cpid_NameAndType, n, desc);
		mtdref = cpaddwords(c, Cpid_Methodref, i, d);
		/* generate code attribute for this.print */
		a = makeattr(Attr_Code, c->code);
		a->info.code.maxstack = 1;
		a->info.code.maxlocals = 1;
		a->info.code.len = 8;
		if ((a->info.code.code = calloc(8, sizeof(byte))) == NULL)
			die(2, "No memory:");
		a->info.code.code[0] = GETSTATIC;
		a->info.code.code[1] = fldref >> 8;
		a->info.code.code[2] = fldref & 0xFF;
		a->info.code.code[3] = ILOAD_0;
		a->info.code.code[4] = mtdref >> 8;
		a->info.code.code[5] = mtdref & 0xFF;
		a->info.code.code[6] = INVOKEVIRTUAL;
		a->info.code.code[7] = RETURN;
		a->info.code.nexceptions = 0;
		a->info.code.nattr = 0;
		setattrsize(a);
		addfield(c, Acc_Private | Acc_Static, name, desc, 1, a, false);
	}
	return c->print;
}

word
getsys(Class *c)
{
	word name;

	if (c->sys == 0) {
		name = cpaddarr(c, Cpid_Utf8, (byte *) STR_SYSTEM_NAME, strlen(STR_SYSTEM_NAME));
		c->sys = cpaddwords(c, Cpid_Class, name, 0);
	}
	return c->sys;
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
makeclass(Buffer *file, const char *classname)
{
	Attribute *a;
	Class *c;
	word desc, i, mref, name;

	c = malloc(sizeof(Class));
	c->file = file;
	c->vmin = JVM_VERSION_MINOR;
	c->vmaj = JVM_VERSION_MAJOR;
	c->print = 0;
	c->read  = 0;
	c->cpool.size = 0;
	c->cpool.list = NULL;
	c->acc = Acc_Super;
	/* Add class to constant pool */
	name = cpaddarr(c, Cpid_Utf8, (byte *) classname, strlen(classname));
	c->this = cpaddwords(c, Cpid_Class, name, 0);
	/* Add super class to constant pool */
	name = cpaddarr(c, Cpid_Utf8, (byte *) STR_OBJECT_NAME, strlen(STR_OBJECT_NAME));
	c->super = cpaddwords(c, Cpid_Class, name, 0);
	c->interfaces.size = 0;
	c->fields.size = 0;
	c->methods.size = 0;
	c->attrs.size = 0;
	/* Add superclass constructor */
	name = cpaddarr(c, Cpid_Utf8, (byte *) STR_CONSTR_NAME, strlen(STR_CONSTR_NAME));
	desc = cpaddarr(c, Cpid_Utf8, (byte *) STR_CONSTR_DESC, strlen(STR_CONSTR_DESC));
	i = cpaddwords(c, Cpid_NameAndType, name, desc);
	mref = cpaddwords(c, Cpid_Methodref, c->super, i);
	/* Add constructor */
	c->code = cpaddarr(c, Cpid_Utf8, (byte *) STR_ATTR_CODE, strlen(STR_ATTR_CODE));
	a = makeattr(Attr_Code, c->code);
	a->info.code.maxstack = 1;
	a->info.code.maxlocals = 1;
	a->info.code.len = DEF_CONSTR_LEN;
	if ((a->info.code.code = calloc(DEF_CONSTR_LEN, sizeof(byte))) == NULL)
		die(2, "No memory:");
	a->info.code.code[0] = ALOAD_0;
	a->info.code.code[1] = INVOKESPECIAL;
	a->info.code.code[2] = mref >> 8;
	a->info.code.code[3] = mref & 0xFF;
	a->info.code.code[4] = RETURN;
	a->info.code.nexceptions = 0;
	a->info.code.nattr = 0;
	setattrsize(a);
	addfield(c, Acc_None, name, desc, 1, a, false);
	/* Add main method (empty) */
	a = makeattr(Attr_Code, c->code);
	a->info.code.maxstack = 1;
	a->info.code.maxlocals = 1;
	a->info.code.len = 5;
	if ((a->info.code.code = calloc(5, sizeof(byte))) == NULL)
		die(2, "No memory:");
	/* just for testing */
	a->info.code.code[0] = ICONST_0;
	a->info.code.code[1] = INVOKESTATIC;
	a->info.code.code[2] = getprint(c) >> 8;;
	a->info.code.code[3] = c->print & 0xFF;
	a->info.code.code[4] = RETURN;
	a->info.code.nexceptions = 0;
	a->info.code.nattr = 0;
	setattrsize(a);
	name = cpaddarr(c, Cpid_Utf8, (byte *) STR_MAIN_NAME, strlen(STR_MAIN_NAME));
	desc = cpaddarr(c, Cpid_Utf8, (byte *) STR_MAIN_DESC, strlen(STR_MAIN_DESC));
	addfield(c, Acc_Public | Acc_Static, name, desc, 1, a, false);
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
			fprintf(stderr, "Cpid_Fieldref: class=%u; nameandtype=%u\n", e->data.words[0], e->data.words[1]);
			writeword(b, e->data.words[0]);
			writeword(b, e->data.words[1]);
			break;
		case Cpid_Methodref:
			fprintf(stderr, "Cpid_Methodref: class=%u; nameandtype=%u\n", e->data.words[0], e->data.words[1]);
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
