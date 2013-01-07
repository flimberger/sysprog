#ifndef _JVM_H_
#define _JVM_H_

#include <stdbool.h>

#include "platform.h"
#include "buffer.h"

/* http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html */

#define STR_ATTR_CODE   "Code"
#define STR_ATTR_LNTBL  "LineNumberTable"
#define STR_CONSTR_DESC "()V"
#define STR_CONSTR_NAME "<init>"
#define STR_MAIN_DESC   "([Ljava/lang/String;)V"
#define STR_MAIN_NAME   "main"
#define STR_OBJECT_NAME "java/lang/Object"
#define STR_OUT_DESC    "Ljava/io/PrintStream;"
#define STR_OUT_NAME    "out"
#define STR_PRNT_DESC   "(I)V"
#define STR_PRNT_NAME   "print"
#define STR_PRNTLN_NAME "println"
#define STR_PRNTSTRM_NAME "java/io/PrintStream"
#define STR_SYSTEM_NAME "java/lang/System"


enum {
	DEF_CONSTR_LEN = 5,
	JVM_VERSION_MAJOR = 51,
	JVM_VERSION_MINOR = 0
};

typedef enum {
	Acc_None,
	Acc_Public     = 0x0001,
	Acc_Private    = 0x0002,
	Acc_Protected  = 0x0004,
	Acc_Static     = 0x0008,
	Acc_Final      = 0x0010,
	Acc_Super      = 0x0020,
	Acc_Synchronized = 0x0020,
	Acc_Volatile   = 0x0040,
	Acc_Bridged    = 0x0040,
	Acc_Transient  = 0x0080,
	Acc_Varargs    = 0x0080,
	Acc_Native     = 0x0100,
	Acc_Interface  = 0x0200,
	Acc_Abstract   = 0x0400,
	Acc_Strict     = 0x0800,
	Acc_Synthetic  = 0x1000,
	Acc_Annotation = 0x2000,
	Acc_Enum       = 0x4000
} Accmod;

typedef enum {
	Attr_Code,
	Attr_Lntbl
} Attrid;

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
	word index;
} SourceFile;

typedef struct {
	word start;
	word line;
} Linenumber;

typedef struct {
	word        len;
	Linenumber *tab;
} LineNumberTable;

typedef struct Attribute Attribute;

typedef struct {
	word start;
	word end;
	word handler;
	word catch;
} ExceptionTab;

typedef struct {
	Attribute *attrs;
	byte *code;
	ExceptionTab *exceptions;
	dword len;
	word maxstack;
	word maxlocals;
	word nattr;
	word nexceptions;
} Code;

struct Attribute {
	Attribute *next;
	union {
		Code    code;
		LineNumberTable linetab;
		SourceFile      sourcefile;
	} info;
	dword len;
	word  name;
	Attrid id;
};

typedef struct Field Field;
struct Field {
	Field *next;
	Attribute *attrs;
	word nattr;
	word acc;
	word name;
	word desc;
};

typedef struct {
	Buffer *file;
	const char *name;
	word vmin;
	word vmaj;
	word print;
	word read;
	word code;
	word sys;
	struct {
		word size;
		Celem *list;
	} cpool;
	word acc;
	word this;
	word super;
	struct {
		word size;
		word *list;
	} interfaces;
	struct {
		word size;
		Field *list;
	} fields;
	struct {
		word size;
		Field *list;
	} methods;
	struct {
		word size;
		Attribute *list;
	} attrs;
} Class;

void addfield(Class *c, word acc, word nameidx, word descidx, word nattr, Attribute *lattr, bool fld);
word cpaddarr(Class *c, Cpoolid id, const byte *const data, word size);
word cpaddtup(Class *c, Cpoolid id, word w, byte b);
word cpaddwords(Class *c, Cpoolid id, word a, word b);
word cpadddwords(Class *c, Cpoolid id, dword a, dword b);
void freeclass(Class *c);
word getprint(Class *c);
word getsys(Class *c);
Attribute *makeattr(Attrid id, word nameidx);
Class* makeclass(Buffer *file, const char *name);
void setattrsize(Attribute *a);
void writeclass(Class *c);

#endif /* _JVM_H_ */
