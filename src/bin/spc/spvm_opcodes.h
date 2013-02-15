#ifndef _SPVM_OPCODES_H_
#define _SPVM_OPCODESH_

typedef enum {
	ADD,
	SUB,
	MUL,
	DIV,
	LES,
	EQU,
	AND,
	NOT,
	LA,
	LC,
	LV,
	STR,
	PRI,
	REA,
	JMP,
	JIN,
	DS,
	NOP,
	STP
} spvm_opcodeid;

extern const char *spvm_op[];

#endif /* _SPVM_OPCODES_H_ */
