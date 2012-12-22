#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "spc.h"

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

static Token *token;

static inline
void
printfunc(char *mesg)
{
	fprintf(stderr, "%u:%u:%s: %s\n", token->row, token->col, tokennames[token->type], mesg);
}

static inline
void
parseerror(const char *const str)
{
	die(EXIT_FAILURE, "%s:%u:%u: Error on Token %s: %s\n", infile, token->row,
			token->col, tokennames[token->type], str);
}

static inline
void
nexttoken(void)
{
	if ((token = gettoken())->type == ERROR)
		parseerror("Error encountered");
	printtoken(token);
}

void
parseprog(void)
{
	fprintf(stderr, "parseprog\n");
	parsetree = makenode();
	nexttoken();
	parsetree->left = parsedecls();
	parsetree->right = parsestatements();
}

static inline
void
parseterm(void)
{
	nexttoken();
	if (token->type != SIGN_TERM)
		parseerror("Expected ;");
}

static
Node *
parsedecls(void)
{
	Node *np;

	np = NULL;
	do {
		printfunc("parsedecls");
		parsedecl();
		parseterm();
		nexttoken();
	} while (token->type == INT);
	return np;
}

static
Node *
parsedecl(void)
{
	Node *np;

	np = NULL;
	printfunc("parsedecl");
	if (token->type != INT)
		parseerror("Expected int keyword");
	nexttoken();
	parsearray();
	if (token->type != IDENTIFIER)
		parseerror("Expected Identifier");
	return np;
}

static
Node *
parsearray(void)
{
	Node *np;

	np = NULL;
	printfunc("parsearray");
	if (token->type != SIGN_BROP)
		return np;
	nexttoken();
	if (token->type != INTEGER)
		parseerror("Expected Integer");
	nexttoken();
	if (token->type != SIGN_BRCL)
		parseerror("Expected ]");
	nexttoken();
	return np;
}

static
Node *
parsestatements(void)
{
	Node *cp, *np, *op, *stmts;

	printfunc("parsestatements");
	np = parsestatement();
	if (np == NULL)
		return NULL;
	parseterm();
	nexttoken();
	if ((cp = parsestatement()) == NULL)
		return np;
	parseterm();
	stmts = makenode();
	stmts->type = NODE_LIST;
	stmts->left = np;
	np = cp;
	op = stmts;
	for ( ; (cp = parsestatement()); np = cp) {
		printfunc("parsestatements");
		parseterm();
		op->right = makenode();
		op->type = NODE_LIST;
		op->left = np;
		nexttoken();
		op = op->right;
	}
	op->right = cp;
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
	switch (token->type) {
	case IDENTIFIER:
		np->left = makenode();
		nexttoken();
		if ((np->left->right = parseindex()) == NULL) {
			np->left->type = NODE_IDENTIFIER;
		} else {
			/* TODO: Set type of np node */
			np->left = makenode();
			np->left->left->type = NODE_IDENTIFIER;
		}
		if (token->type != SIGN_EQUAL)
			parseerror("Expected =");
		np->type = NODE_OPERATOR;
		np->data.operator = OP_EQUAL;		
		nexttoken();
		np->right = parseexp();
		break;
	case PRINT:
		np->type = NODE_PRINT;
		nexttoken();
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		np->type = NODE_PRINT;		
		nexttoken();
		np->left = parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		break;
	case READ:
		np->type = NODE_READ;
		nexttoken();
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		np->left = makenode();
		nexttoken();
		if ((np->left->right = parseindex()) == NULL) {
			np->left->type = NODE_IDENTIFIER;
		} else {
			/* TODO: Set type of np node */
			np->left = makenode();
			np->left->left->type = NODE_IDENTIFIER;
		}	
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");		
		break;
	case SIGN_CBOP:
		nexttoken();
		if (token->type != SIGN_BROP)
			parseerror("Expected {");
		np = parsestatements();
		if (token->type != SIGN_BRCL)
			parseerror("Expected }");
		break;
	case IF:
		np->type = NODE_IF;
		nexttoken();
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		np->left = parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		np->right = makenode();
		np->right->left = parsestatement();
		if (token->type != ELSE)
			parseerror("Expected else");
		nexttoken();
		np->right->right = parsestatement();
		break;
	case WHILE:
		np->type = NODE_WHILE;
		nexttoken();
		if (token->type != WHILE)
			parseerror("Expected while");
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		np->left = parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		nexttoken();
		np->right = parsestatement();
		break;
	default:
		parseerror("Expected Identifier, if, while or }");
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
	if (token->type != SIGN_BROP)
		return NULL;
	nexttoken();
	np = parseexp();
	nexttoken();
	if (token->type != SIGN_BRCL)
		parseerror("Expected ]");
	return np;
}

static
Node *
parseexp(void)
{
	Node *npopexp, *npexp2;

	printfunc("parseexp");
	npexp2 = parseexp2();
	nexttoken();
	if ((npopexp = parseop_exp()) == NULL) {
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
	switch (token->type) {
	case SIGN_PAROP:
		nexttoken();
		np = parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected (");
		break;
	case IDENTIFIER:
		np = makenode();		
		nexttoken();
		if ((np->right = parseindex()) == NULL) {
			np->type = NODE_IDENTIFIER;
		} else {
			/* TODO: Set type of np node */
			np->left = makenode();
			np->left->type = NODE_IDENTIFIER;
		}
		break;
	case INTEGER:
		np = makenode();
		np->type = NODE_CONSTANT;
		break;
	case SIGN_MINUS:
	case SIGN_NOT:
		np = makenode();
		np->type = NODE_OPERATOR;
		if (token->type == SIGN_MINUS) {
			np->data.operator = OP_SUB;
		} else {
			np->data.operator = OP_NOT;
		}		
		nexttoken();
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
		nexttoken();
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
	switch (token->type) {
	case SIGN_PLUS:
		np->data.operator = OP_ADD;
		break;
	case SIGN_MINUS:
		np->data.operator = OP_SUB;
		break;
	case SIGN_MULT:
		np->data.operator = OP_MUL;
		break;
	case SIGN_DIV:
		np->data.operator = OP_DIV;
		break;
	case SIGN_LESS:
		np->data.operator = OP_LESS;
		break;
	case SIGN_GRTR:
		np->data.operator = OP_GRTR;
		break;
	case SIGN_EQUAL:
		np->data.operator = OP_EQUAL;
		break;
	case SIGN_UNEQL:
		np->data.operator = OP_UNEQ;
		break;
	case SIGN_AND:
		np->data.operator = OP_AND;
		break;
	default:
		free(np);
		return NULL;
	}
	return np;
}
