#ifndef _OPCODES_H_
#define _OPCODES_H_

/* http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-6.html */

typedef enum {
	LDC = 0x12,

	ALOAD_0 = 0x2a,
	ALOAD_1 = 0x2b,
	ALOAD_2 = 0x2c,
	ALOAD_3 = 0x2d,

	RETURN = 0xb1,
	GETSTATIC = 0xb2,

	INVOKEVIRTUAL = 0xb6,
	INVOKESPECIAL = 0xb7,
	INVOKESTATIC  = 0xb8,
} Opcode;

extern char *opcodenames[];

#endif /* _OPCODES_H_ */
