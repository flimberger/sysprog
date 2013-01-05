#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "spc.h"

#define FIRST_STATEMENT ( \
	(nexttoken->type == IDENTIFIER) || (nexttoken->type == PRINT) || \
	(nexttoken->type == READ) || (nexttoken->type == SIGN_CBOP) || \
	(nexttoken->type == IF) || (nexttoken->type == WHILE) \
)

#define FIRST_OP_EXP ( \
	(nexttoken->type == SIGN_PLUS) || (nexttoken->type == SIGN_MINUS) || \
	(nexttoken->type == SIGN_MULT) || (nexttoken->type == SIGN_DIV) || \
	(nexttoken->type == SIGN_GRTR) || (nexttoken->type == SIGN_LESS) || \
	(nexttoken->type == SIGN_EQUAL) || (nexttoken->type == SIGN_UNEQL) || \
	(nexttoken->type == SIGN_AND) \
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
		tokennames[nexttoken->type], mesg);
}

static
void
match(Symboltype t)
{
	if (t == nexttoken->type) {
		printtoken(nexttoken);
		nexttoken = gettoken();
	} else
		die(EXIT_FAILURE, "%s:%u:%u: Error on Token %s: Expected %s\n",
			infile, nexttoken->row, nexttoken->col,
			tokennames[nexttoken->type], tokennames[t]);
}

void
parseprog(void)
{
	fprintf(stderr, "parseprog: Begin parsing\n");
	nexttoken = gettoken();
	parsetree = makenode();
	parsetree->left = parsedecls();
	parsetree->right = parsestatements();
}

static
Node *
parsedecls(void)
{
	Node *decls, *lp, *np;

	printfunc("parsedecls");
	if (nexttoken->type != INT) {
		fprintf(stderr, "no decls\n");
		return NULL;
	}
	np = parsedecl();
	match(SIGN_TERM);
	if (nexttoken->type != INT) {
		fprintf(stderr, "only one decl\n");
		return np;
	}
	fprintf(stderr, "more decls\n");
	lp = makenode();
	lp->type = NODE_LIST;
	lp->left = np;
	np = parsedecl();
	match(SIGN_TERM);
	decls = lp;
	while (nexttoken->type == INT) {
		printfunc("parsedecls");
		lp->right = makenode();
		lp = lp->right;
		lp->type = NODE_LIST;
		lp->left = np;
		np = parsedecl();
		match(SIGN_TERM);
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

	np = NULL;
	printfunc("parsedecl");
	match(INT);
	if (nexttoken->type == SIGN_BROP)
		np = parsearray();
	match(IDENTIFIER);
	if (np != NULL)
		np->left = NULL; /* TODO: identifier */
	else {
		np = makenode();
		np->type = NODE_IDENTIFIER;
	}
	return np;
}

/*
 *    Array
 *    /   \
 * name  length
 */
static
Node *
parsearray(void)
{
	Node *np;

	printfunc("parsearray");
	match(SIGN_BROP);
	match(INTEGER);
	match(SIGN_BRCL);
	np = makenode();
	np->type = NODE_ARRAY;
	return np;
}

static
Node *
parsestatements(void)
{
	Node *lp, *np, *stmts;

	printfunc("parsestatements");
	np = parsestatement();
	match(SIGN_TERM);
	if (!FIRST_STATEMENT)
		return np;
	lp = makenode();
	lp->type = NODE_LIST;
	lp->left = np;
	np = parsestatement();
	match(SIGN_TERM);
	stmts = lp;
	while (FIRST_STATEMENT) {
		printfunc("parsestatements");
		lp->right = makenode();
		lp = lp->right;
		lp->type = NODE_LIST;
		lp->left = np;
		np = parsestatement();
		match(SIGN_TERM);
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
	switch (nexttoken->type) {
	case IDENTIFIER:
		match(IDENTIFIER);
		if (nexttoken->type == SIGN_BROP)
			np->left = parseindex();
		match(SIGN_EQUAL);
		np->type = NODE_OPERATOR;
		np->data.op = OP_EQUAL;
		np->right = parseexp();
		break;
	case PRINT:
		match(PRINT);
		np->type = NODE_PRINT;
		match(SIGN_PAROP);
		np->left = parseexp();
		match(SIGN_PARCL);
		break;
	case READ:
		match(READ);
		np->type = NODE_READ;
		match(SIGN_PAROP);
		match(IDENTIFIER);
		if (nexttoken->type == SIGN_BROP) {
			np->left = makenode();
			np->left->right = parseindex();
			np->left->type = NODE_IDENTIFIER;
		}
		match(SIGN_PARCL);
		break;
	case SIGN_CBOP:
		match(SIGN_CBOP);
		np = parsestatements();
		match(SIGN_CBCL);
		break;
	case IF:
		match(IF);
		np->type = NODE_IF;
		match(SIGN_PAROP);
		np->left = parseexp();
		match(SIGN_PARCL);
		np->right = makenode();
		np->right->left = parsestatement();
		match(ELSE);
		np->right->right = parsestatement();
		break;
	case WHILE:
		match(WHILE);
		np->type = NODE_WHILE;
		match(SIGN_PAROP);
		np->left = parseexp();
		match(SIGN_PARCL);
		np->right = parsestatement();
		break;
	default:
		die(EXIT_FAILURE,
			"%s:%u:%u: Error on Token %s: Expected Expected Identifier, if, while or {\n",
			infile, nexttoken->row, nexttoken->col,
			tokennames[nexttoken->type]);
	}
	return np;
}

/* TODO: Fix index node */
static
Node *
parseindex(void)
{
	Node *np;

	printfunc("parseindex");
	match(SIGN_BROP);
	np = parseexp();
	match(SIGN_BRCL);
	return np;
}

static
Node *
parseexp(void)
{
	Node *npopexp, *npexp2;

	printfunc("parseexp");
	npexp2 = parseexp2();
	if (FIRST_OP_EXP) {
		npopexp = parseop_exp();
		npopexp->left = npexp2;
		return npopexp;
	}
	return npexp2;	
}

static
Node *
parseexp2(void)
{
	Node *np;

	printfunc("parseexp2");
	switch (nexttoken->type) {
	case SIGN_PAROP:
		match(SIGN_PAROP);
		np = parseexp();
		match(SIGN_PARCL);
		break;
	case IDENTIFIER:
		match(IDENTIFIER);
		np = makenode();
		np->type = NODE_IDENTIFIER;
		if (nexttoken->type == SIGN_BROP)
			np->right = parseindex();
		break;
	case INTEGER:
		match(INTEGER);
		np = makenode();
		np->type = NODE_CONSTANT;
		break;
	case SIGN_MINUS:
	case SIGN_NOT:
		np = makenode();
		np->type = NODE_OPERATOR;
		if (nexttoken->type == SIGN_MINUS) {
			match(SIGN_MINUS);
			np->data.op = OP_SUB;
		} else {
			match(SIGN_NOT);
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
	if ((np = parseop()) != NULL) {
		np->right = parseexp();
		return np;
	}
	return NULL;
}

static
Node *
parseop(void)
{
	Node *np;

	printfunc("parseop");
	np = makenode();
	np->type = NODE_OPERATOR;
	switch (nexttoken->type) {
	case SIGN_PLUS:
		match(SIGN_PLUS);
		np->data.op = OP_ADD;
		break;
	case SIGN_MINUS:
		match(SIGN_MINUS);
		np->data.op = OP_SUB;
		break;
	case SIGN_MULT:
		match(SIGN_MULT);
		np->data.op = OP_MUL;
		break;
	case SIGN_DIV:
		match(SIGN_DIV);
		np->data.op = OP_DIV;
		break;
	case SIGN_LESS:
		match(SIGN_LESS);
		np->data.op = OP_LESS;
		break;
	case SIGN_GRTR:
		match(SIGN_GRTR);
		np->data.op = OP_GRTR;
		break;
	case SIGN_EQUAL:
		match(SIGN_EQUAL);
		np->data.op = OP_EQUAL;
		break;
	case SIGN_UNEQL:
		match(SIGN_UNEQL);
		np->data.op = OP_UNEQ;
		break;
	case SIGN_AND:
		match(SIGN_AND);
		np->data.op = OP_AND;
		break;
	default:
		free(np);
		return NULL;
	}
	return np;
}
