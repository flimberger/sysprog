#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "buffer.h"
#include "defs.h"
#include "symtab.h"

typedef enum {
	NODE_ARRAY,
	NODE_CONSTANT,
	NODE_IDENTIFIER,
	NODE_IF,
	NODE_LIST,
	NODE_OPERATOR,
	NODE_PRINT,
	NODE_READ,
	NODE_WHILE
} Nodetype;

typedef enum {
	OP_ADD,	/* + */
	OP_SUB,	/* - */
	OP_DIV,	/* / */
	OP_MUL,	/* * */
	OP_LESS,	/* < */
	OP_GRTR,	/* > */
	OP_EQUAL,	/* = */
	OP_UNEQ,	/* <!> */
	OP_NOT,	/* ! */
	OP_AND	/* & */
} Optype;

typedef struct {
	union {
		long long val;
		Symbol *sym;
		const char *sign;
		char lastchar;
	} data;
	uint	row;
	uint	col;
	Symboltype	type;
} Token;

typedef struct Node Node;
struct Node {
	Node *left;
	Node *right;
	Nodetype type;
	union {
		vlong intval;
		Symbol *sym;
		Optype operator;
	} data;
};

extern char *infile;
extern const char *const tokennames[];
extern Buffer *src, *out;
extern Symtab *symtab;
extern Node *parsetree;

int compile(char *outfile);
Token *gettoken(void);
void parseprog(void);
void printtoken(Token *tp);
void syminit(void);
void symterm(void);

// Btree functions
Node *makenode(void);

#endif /* _SYSPROG_H_ */
