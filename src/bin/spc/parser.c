#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "spc.h"

#define FIRST_STATEMENT ( \
	(nexttoken->symtype == S_IDENT) || (nexttoken->symtype == S_PRINT) || \
	(nexttoken->symtype == S_READ) || (nexttoken->symtype == S_CBOP) || \
	(nexttoken->symtype == S_IF) || (nexttoken->symtype == S_WHILE) \
)

#define FIRST_OP_EXP ( \
	(nexttoken->symtype == S_PLUS) || (nexttoken->symtype == S_MINUS) || \
	(nexttoken->symtype == S_MULT) || (nexttoken->symtype == S_DIV) || \
	(nexttoken->symtype == S_GRTR) || (nexttoken->symtype == S_LESS) || \
	(nexttoken->symtype == S_EQUAL) || (nexttoken->symtype == S_UNEQL) || \
	(nexttoken->symtype == S_AND) \
)

static Node* parsedecls(void);
static Node* parsedecl(void);
static Node* parsearray(void);
static Node* parsestatements(void);
static Node* parsestatement(void);
static Node* parseindex(void);
static Node* parseexp(void);
static Node* parseexp2(void);
static Node* parseop_exp(void);
static Node* parseop(void);

static Token *nexttoken;

static inline
void
printfunc(char *mesg)
{
	fprintf(stderr, "%u:%u:%s: %s\n", nexttoken->row, nexttoken->col,
		tokennames[nexttoken->symtype], mesg);
}

static inline
void
match(Symboltype t)
{
	if (nexttoken->symtype == t)
		nexttoken = gettoken();
	else
		die(EXIT_FAILURE, "%s:%u:%u: Error on Token %s: Expected %s\n",
			infile, nexttoken->row, nexttoken->col,
			tokennames[nexttoken->symtype], tokennames[t]);
}

void
parseprog(void)
{
	fprintf(stderr, "parseprog: Begin parsing\n");
	nexttoken = gettoken();
	parsetree = makenode(NODE_PROG, nexttoken->row, nexttoken->col);
	parsetree->left = parsedecls();
	parsetree->right = parsestatements();
}

static
Node *
parsedecls(void)
{
	Node *node;

	printfunc("parsedecls");
	if (nexttoken->symtype != S_INT) {
		return NULL;
	}

	node = makenode(NODE_DECLS, nexttoken->row, nexttoken->col);
	node->left = parsedecl();
	match(S_TERM);
	node->right = parsedecls();
	return node;
}

static
Node *
parsedecl(void)
{
	Node *node;

	printfunc("parsedecl");
	node = makenode(NODE_DECL, nexttoken->row, nexttoken->col);
	match(S_INT);
	if (nexttoken->symtype == S_BROP)
		node->left = parsearray();
	
	if (nexttoken->symtype == S_IDENT)
		node->data.sym = nexttoken->data.sym;
	match(S_IDENT);
	return node;
}

static
Node *
parsearray(void)
{
	Node *node;

	printfunc("parsearray");
	match(S_BROP);
	if (nexttoken->symtype == S_INTCONST) {
		node = makenode(NODE_ARRAY, nexttoken->row, nexttoken->col);
		node->data.val = nexttoken->data.val;
	}
	match(S_INTCONST);
	match(S_BRCL);
	return node;
}

static
Node *
parsestatements(void)
{
	Node *node;

	printfunc("parsestatements");	
	if (!FIRST_STATEMENT)
		return NULL;
	node = makenode(NODE_STATEMENTS, nexttoken->row, nexttoken->col);
	node->left = parsestatement();
	match(S_TERM);
	node->right = parsestatements();
	return node;
}

static
Node *
parsestatement(void)
{
	Node *node;

	printfunc("parsestatement");
	node = makenode(NODE_STATEMENT, nexttoken->row, nexttoken->col);
	#pragma GCC diagnostic ignored "-Wswitch"
	switch (nexttoken->symtype) {
	case S_IDENT:
		node->left = makenode(NODE_IDENT, nexttoken->row, nexttoken->col);
		node->left->data.sym = nexttoken->data.sym;
		match(S_IDENT);
		node->right = makenode(NODE_NONE, nexttoken->row, nexttoken->col);
		if (nexttoken->symtype == S_BROP)
			node->right->left = parseindex();
		match(S_EQUAL);
		node->right->right = parseexp();
		break;
	case S_PRINT:
		node->left = makenode(NODE_PRINT, nexttoken->row, nexttoken->col);
		match(S_PRINT);
		match(S_PAROP);
		node->left->left = parseexp();
		match(S_PARCL);
		break;
	case S_READ:
		node->left = makenode(NODE_READ, nexttoken->row, nexttoken->col);
		match(S_READ);
		match(S_PAROP);
		if (nexttoken->symtype == S_IDENT) {
			node->left->left = makenode(NODE_IDENT, nexttoken->row, nexttoken->col);
			node->left->left->data.sym = nexttoken->data.sym;
		}
		match(S_IDENT);
		if (nexttoken->symtype == S_BROP)
			node->left->right = parseindex();
		match(S_PARCL);
		break;
	case S_CBOP:
		match(S_CBOP);
		node->left = parsestatements();
		match(S_CBCL);
		break;
	case S_IF:
		node->left = makenode(NODE_IF, nexttoken->row, nexttoken->col);
		match(S_IF);
		match(S_PAROP);
		node->right = parseexp();
		match(S_PARCL);
		node->left->left = parsestatement();
		match(S_ELSE);
		node->left->right = parsestatement();
		break;
	case S_WHILE:
		node->left = makenode(NODE_WHILE, nexttoken->row, nexttoken->col);
		match(S_WHILE);
		match(S_PAROP);
		node->right = parseexp();
		match(S_PARCL);
		node->left->left = parsestatement();
		break;
	default:
		die(EXIT_FAILURE,
			"%s:%u:%u: Error on Token %s: Expected S_IDENT, if, while or {\n",
			infile, nexttoken->row, nexttoken->col,
			tokennames[nexttoken->symtype]);
	}
	return node;
}

static
Node *
parseindex(void)
{
	Node *node;

	printfunc("parseindex");
	node = makenode(NODE_INDEX, nexttoken->row, nexttoken->col);
	match(S_BROP);	
	node->left = parseexp();
	match(S_BRCL);
	return node;
}

static
Node *
parseexp(void)
{
	Node *node;

	printfunc("parseexp");
	node = makenode(NODE_EXP, nexttoken->row, nexttoken->col);
	node->left = parseexp2();
	if (FIRST_OP_EXP) {
		node->right = parseop_exp();
	}
	return node;
}

static
Node *
parseexp2(void)
{
	Node *node;

	node = makenode(NODE_EXP2, nexttoken->row, nexttoken->col);
	printfunc("parseexp2");
	switch (nexttoken->symtype) {
	case S_PAROP:
		match(S_PAROP);
		node->left = parseexp();
		match(S_PARCL);
		break;
	case S_IDENT:
		node->left = makenode(NODE_IDENT, nexttoken->row, nexttoken->col);
		node->left->data.sym = nexttoken->data.sym;
		match(S_IDENT);
		if (nexttoken->symtype == S_BROP)
			node->right = parseindex();
		break;
	case S_INTCONST:		
		node->left = makenode(NODE_INTCONST, nexttoken->row, nexttoken->col);
		node->left->data.val = nexttoken->data.val;
		match(S_INTCONST);
		break;
	case S_MINUS:
	case S_NOT:
		node->left = makenode(NODE_OP, nexttoken->row, nexttoken->col);
		if (nexttoken->symtype == S_MINUS) {
			match(S_MINUS);
			node->left->data.op = OP_NEG;
		} else {
			match(S_NOT);
			node->left->data.op = OP_NOT;
		}
		node->right = parseexp2();
		break;
	}
	return node;
}

static
Node *
parseop_exp(void)
{
	Node *node;

	node = makenode(NODE_OPEXP, nexttoken->row, nexttoken->col);
	printfunc("parseop_exp");
	node->left = parseop();
	node->right = parseexp();
	return node;
}

static
Node *
parseop(void)
{
	Node *node;

	printfunc("parseop");
	node = makenode(NODE_OP, nexttoken->row, nexttoken->col);
	node->col = nexttoken->col;
	node->row = nexttoken->row;
	switch (nexttoken->symtype) {
	case S_PLUS:
		match(S_PLUS);
		node->data.op = OP_ADD;
		break;
	case S_MINUS:
		match(S_MINUS);
		node->data.op = OP_SUB;
		break;
	case S_MULT:
		match(S_MULT);
		node->data.op = OP_MUL;
		break;
	case S_DIV:
		match(S_DIV);
		node->data.op = OP_DIV;
		break;
	case S_LESS:
		match(S_LESS);
		node->data.op = OP_LESS;
		break;
	case S_GRTR:
		match(S_GRTR);
		node->data.op = OP_GRTR;
		break;
	case S_EQUAL:
		match(S_EQUAL);
		node->data.op = OP_EQUAL;
		break;
	case S_UNEQL:
		match(S_UNEQL);
		node->data.op = OP_UNEQ;
		break;
	case S_AND:
		match(S_AND);
		node->data.op = OP_AND;
		break;
	}
	return node;
}
