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

static
void
match(Symboltype t)
{
	if (nexttoken->symtype == t) {
		printtoken(nexttoken);
		nexttoken = gettoken();
	} else
		die(EXIT_FAILURE, "%s:%u:%u: Error on Token %s: Expected %s\n",
			infile, nexttoken->row, nexttoken->col,
			tokennames[nexttoken->symtype], tokennames[t]);
}

void
parseprog(void)
{
	fprintf(stderr, "parseprog: Begin parsing\n");
	nexttoken = gettoken();
	parsetree = makenode();
	parsetree->type = NODE_ROOT;
	parsetree->left = parsedecls();
	parsetree->right = parsestatements();
}

static
Node *
parsedecls(void)
{
	Node *decls, *lp, *np;

	printfunc("parsedecls");
	if (nexttoken->symtype != S_INT) {
		fprintf(stderr, "no decls\n");
		return NULL;
	}
	np = parsedecl();
	match(S_TERM);
	if (nexttoken->symtype != S_INT) {
		fprintf(stderr, "only one decl\n");
		return np;
	}
	fprintf(stderr, "more decls\n");
	lp = makenode();
	lp->type = NODE_LIST;
	lp->left = np;
	np = parsedecl();
	match(S_TERM);
	decls = lp;
	while (nexttoken->symtype == S_INT) {
		printfunc("parsedecls");
		lp->right = makenode();
		lp = lp->right;
		lp->type = NODE_LIST;
		lp->left = np;
		np = parsedecl();
		match(S_TERM);
	}
	lp->right = np;
	fprintf(stderr, "done parsing decls\n");
	return decls;
}

static
Node *
parsedecl(void)
{
	Node *np;

	printfunc("parsedecl");
	match(S_INT);
	np = makenode();
	np->type = NODE_DECL;
	if (nexttoken->symtype == S_BROP) {
		np->left = parsearray();
		np->datatype = T_INTARR;
	} else
		np->datatype = T_INT;
	if (nexttoken->symtype == S_IDENT) {
		np->data.sym = nexttoken->data.sym;
		np->data.sym->datatype = np->datatype;
	}
	match(S_IDENT);
	return np;
}

static
Node *
parsearray(void)
{
	Node *np;

	printfunc("parsearray");
	match(S_BROP);
	if (nexttoken->symtype == S_INTCONST) {
		np = makenode();
		np->type = NODE_ARRAY;
		np->data.val = nexttoken->data.val;
	}
	match(S_INTCONST);
	match(S_BRCL);
	return np;
}

static
Node *
parsestatements(void)
{
	Node *lp, *np, *stmts;

	printfunc("parsestatements");
	np = parsestatement();
	match(S_TERM);
	if (!FIRST_STATEMENT)
		return np;
	lp = makenode();
	lp->type = NODE_LIST;
	lp->left = np;
	np = parsestatement();
	match(S_TERM);
	stmts = lp;
	while (FIRST_STATEMENT) {
		printfunc("parsestatements");
		lp->right = makenode();
		lp = lp->right;
		lp->type = NODE_LIST;
		lp->left = np;
		np = parsestatement();
		match(S_TERM);
	}
	lp->right = np;
	return stmts;
}

static
Node *
parsestatement(void)
{
	Node *np;

	np = makenode();
	printfunc("parsestatement");
	#pragma GCC diagnostic ignored "-Wswitch"
	switch (nexttoken->symtype) {
	case S_IDENT:
		np->data.sym = nexttoken->data.sym;
		match(S_IDENT);
		if (nexttoken->symtype == S_BROP)
			np->left = parseindex();
		match(S_EQUAL);
		np->type = NODE_ASSGN;
		np->right = parseexp();
		break;
	case S_PRINT:
		match(S_PRINT);
		np->type = NODE_PRINT;
		match(S_PAROP);
		np->left = parseexp();
		match(S_PARCL);
		break;
	case S_READ:
		match(S_READ);
		np->type = NODE_READ;
		match(S_PAROP);
		if (nexttoken->symtype == S_IDENT) {
			np->left = makenode();
			np->left->type = NODE_IDENT;
			np->left->data.sym = nexttoken->data.sym;
		}
		match(S_IDENT);
		if (nexttoken->symtype == S_BROP)
			np->left->right = parseindex();
		match(S_PARCL);
		break;
	case S_CBOP:
		match(S_CBOP);
		np = parsestatements();
		match(S_CBCL);
		break;
	case S_IF:
		match(S_IF);
		np->type = NODE_IF;
		match(S_PAROP);
		np->left = parseexp();
		match(S_PARCL);
		np->right = makenode();
		np->right->type = NODE_NONE;
		np->right->left = parsestatement();
		match(S_ELSE);
		np->right->right = parsestatement();
		break;
	case S_WHILE:
		match(S_WHILE);
		np->type = NODE_WHILE;
		match(S_PAROP);
		np->left = parseexp();
		match(S_PARCL);
		np->right = parsestatement();
		break;
	default:
		die(EXIT_FAILURE,
			"%s:%u:%u: Error on Token %s: Expected Expected S_IDENT, if, while or {\n",
			infile, nexttoken->row, nexttoken->col,
			tokennames[nexttoken->symtype]);
	}
	return np;
}

static
Node *
parseindex(void)
{
	Node *np;

	printfunc("parseindex");
	match(S_BROP);
	np = parseexp();
	match(S_BRCL);
	return np;
}

static
Node *
parseexp(void)
{
	Node *np, *op;

	printfunc("parseexp");
	np = parseexp2();
	if (FIRST_OP_EXP) {
		op = parseop_exp();
		op->left = np;
		return op;
	}
	return np;
}

static
Node *
parseexp2(void)
{
	Node *np;

	printfunc("parseexp2");
	switch (nexttoken->symtype) {
	case S_PAROP:
		match(S_PAROP);
		np = parseexp();
		match(S_PARCL);
		break;
	case S_IDENT:
		np = makenode();
		np->data.sym = nexttoken->data.sym;
		match(S_IDENT);
		np->type = NODE_IDENT;
		if (nexttoken->symtype == S_BROP)
			np->left = parseindex();
		break;
	case S_INTCONST:
		np = makenode();
		np->type = NODE_CONST;
		np->data.val = nexttoken->data.val;
		match(S_INTCONST);
		break;
	case S_MINUS:
	case S_NOT:
		np = makenode();
		np->type = NODE_OP;
		if (nexttoken->symtype == S_MINUS) {
			match(S_MINUS);
			np->data.op = OP_NEG;
		} else {
			match(S_NOT);
			np->data.op = OP_NOT;
		}
		np->left = parseexp2();
		break;
	}
	return np;
}

static
Node *
parseop_exp(void)
{
	Node *np;

	printfunc("parseop_exp");
	np = parseop();
	np->right = parseexp();
	return np;
}

static
Node *
parseop(void)
{
	Node *np;

	printfunc("parseop");
	np = makenode();
	np->type = NODE_OP;
	switch (nexttoken->symtype) {
	case S_PLUS:
		match(S_PLUS);
		np->data.op = OP_ADD;
		break;
	case S_MINUS:
		match(S_MINUS);
		np->data.op = OP_SUB;
		break;
	case S_MULT:
		match(S_MULT);
		np->data.op = OP_MUL;
		break;
	case S_DIV:
		match(S_DIV);
		np->data.op = OP_DIV;
		break;
	case S_LESS:
		match(S_LESS);
		np->data.op = OP_LESS;
		break;
	case S_GRTR:
		match(S_GRTR);
		np->data.op = OP_GRTR;
		break;
	case S_EQUAL:
		match(S_EQUAL);
		np->data.op = OP_EQUAL;
		break;
	case S_UNEQL:
		match(S_UNEQL);
		np->data.op = OP_UNEQ;
		break;
	case S_AND:
		match(S_AND);
		np->data.op = OP_AND;
		break;
	}
	return np;
}
