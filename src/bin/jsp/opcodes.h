#ifndef _OPCODES_H_
#define _OPCODES_H_

/* http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html */

typedef enum {
	NOP       = 0x00,
	ACONST_NULL = 0x01,
	ICONST_M1 = 0x02,
	ICONST_0  = 0x03,
	ICONST_1  = 0x04,
	ICONST_2  = 0x05,
	ICONST_3  = 0x06,
	ICONST_4  = 0x07,
	ICONST_5  = 0x08,

	LDC = 0x12,

	ILOAD = 0x15,

	ILOAD_0 = 0x1a,
	ILOAD_1 = 0x1b,
	ILOAD_2 = 0x1c,
	ILOAD_3 = 0x1d,

	ALOAD_0 = 0x2a,
	ALOAD_1 = 0x2b,
	ALOAD_2 = 0x2c,
	ALOAD_3 = 0x2d,

	ISTORE = 0x36,

	ASTORE   = 0x3a,
	ISTORE_0 = 0x3b,
	ISTORE_1 = 0x3c,
	ISTORE_2 = 0x3d,
	ISTORE_3 = 0x3e,

	ASTORE_0 = 0x4b,
	ASTORE_1 = 0x4c,
	ASTORE_2 = 0x4d,
	ASTORE_3 = 0x4e,

	DUP = 0x59,

	GOTO = 0xa7,

	IRETURN = 0xac,

	RETURN = 0xb1,
	GETSTATIC = 0xb2,

	INVOKEVIRTUAL = 0xb6,
	INVOKESPECIAL = 0xb7,
	INVOKESTATIC  = 0xb8,

	NEW = 0xbb
} Opcode;

extern char *opcodenames[];

#endif /* _OPCODES_H_ */
