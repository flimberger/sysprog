#include "opcodes.h"

char *opcodenames[] = {
/* 0x00 */	"nop",
/* 0x01 */	"aconst_null"
/* 0x02 */	"iconst_m1",
/* 0x03 */	"iconst_0",
/* 0x04 */	"iconst_1",
/* 0x05 */	"iconst_2",
/* 0x06 */	"iconst_3",
/* 0x07 */	"iconst_4",
/* 0x08 */	"iconst_5",

/* 0x12 */	"ldc",

/* 0x15 */	"iload",

/* 0x1a */	"iload_0",
/* 0x1b */	"iload_1",
/* 0x1c */	"iload_2",
/* 0x1d */	"iload_3",

/* 0x2a */	"aload_0",
/* 0x2b */	"aload_1",
/* 0x2c */	"aload_2",
/* 0x2c */	"aload_3",

/* 0x36 */	"istore",

/* 0x3a */	"astore",
/* 0x3b */	"istore_0",
/* 0x3c */	"istore_1",
/* 0x3d */	"istore_2",
/* 0x3e */	"istore_3",

/* 0x4b */	"astore_0",
/* 0x4c */	"astore_1",
/* 0x4d */	"astore_2",
/* 0x4e */	"astore_3",

/* 0xa7 */	"goto",

/* 0x59 */	"dup",

/* 0xac */	"ireturn",

/* 0xb1 */	"return",
/* 0xb2 */	"getstatic",

/* 0xb6 */	"invokevirtual",
/* 0xb7 */	"invokespecial",
/* 0xb8 */	"invokestatic",

/* 0xbb */	"new"
};
