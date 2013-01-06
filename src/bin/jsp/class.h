#ifndef _JVM_H_
#define _JVM_H_

#include "platform.h"
#include "buffer.h"

enum {
	JVM_VERSION_MAJOR = 51,
	JVM_VERSION_MINOR = 0
};

typedef enum {
	Cpid_Utf8 = 1,

	Cpid_Integer = 3,
	Cpid_Float,
	Cpid_Long,
	Cpid_Double,
	Cpid_Class,
	Cpid_String,
	Cpid_Fieldref,
	Cpid_Methodref,
	Cpid_InterfaceMethodref,
	Cpid_NameAndType,

	Cpid_MethodHandle = 15,
	Cpid_MethodType,

	Cpid_InvokeDynamic = 18
} Cpoolid;

typedef struct {
	const byte *data;
	word size;
} Bytearr;

typedef struct {
	word w;
	byte b;
} Bwtuple;

typedef struct Celem Celem;
struct Celem {
	Celem *next;
	union {
		Bytearr array;
		Bwtuple tuple;
		word    words[2];
		dword   dwords[2];
	} data;
	Cpoolid id;
};

typedef struct {
	Buffer *file;
	const char *name;
	word vmin;
	word vmaj;
	struct {
		word size;
		Celem *list;
	} cpool;
} Class;

void cpaddarr(Class *c, Cpoolid id, const byte *const data, word size);
void cpaddtup(Class *c, Cpoolid id, word w, byte b);
void cpaddwords(Class *c, Cpoolid id, word a, word b);
void cpadddwords(Class *c, Cpoolid id, dword a, dword b);
Class* makeclass(Buffer *file, const char *name);
void writeclass(Class *c);

#endif /* _JVM_H_ */
