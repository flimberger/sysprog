#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "buffer.h"
#include "defs.h"
#include "symtab.h"

typedef enum {
	NODE_NONE,
	/* nonterminal symbols */
	NODE_PROG,
	NODE_DECLS,
	NODE_DECL,
	NODE_ARRAY,
	NODE_STATEMENTS,
	NODE_STATEMENT,
	NODE_EXP,
	NODE_EXP2,
	NODE_INDEX,
	NODE_OPEXP,
	NODE_OP,
	/* terminal symbols */
	NODE_PRINT,
	NODE_READ,
	NODE_IF,
	NODE_WHILE,
	NODE_IDENT,
	NODE_INTCONST
} Nodetype;

typedef enum {
	OP_NEG,	/* unary - */
	OP_ADD,	/* + */
	OP_SUB,	/* - */
	OP_DIV,	/* / */
	OP_MUL,	/* * */
	OP_LESS,	/* < */
	OP_GRTR,	/* > */
	OP_EQUAL,	/* = */
	OP_UNEQ,	/* <!> */
	OP_NOT,	/* ! (unary) */
	OP_AND	/* & */
} Optype;

typedef struct {
	union {
		vlong  val;
		Symbol *sym;
		const char *sign;
		char   lastchar;
	} data;
	uint row;
	uint col;
	Symboltype symtype;
} Token;

typedef struct Node Node;
struct Node {
	Node *left;
	Node *right;
	Nodetype type;
	union {
		vlong  val;
		Symbol *sym;
		Optype op;
	} data;
	Datatype datatype;
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

/* btree.c */
Node *makenode(Nodetype type);

/* typecheck.c */
void checkprog(Node *node);

/* codegen.c */
void genprog(Node *node);

#endif /* _SYSPROG_H_ */
