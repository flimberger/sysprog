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
	if ((parsetree->left = parsedecls()) == NULL)
		parseerror("FIXME parsedecls");
	if ((parsetree->right = parsestatements()) == NULL)
		parseerror("FIXME parsestatements");
}

static
Node *
parsedecls(void)
{
	Node *np;

	np = NULL;
	do {
		fprintf(stderr, "parsedecls\n");
		parsedecl();
		nexttoken();
		if (token->type != SIGN_TERM)
			parseerror("Expected ;");
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
	fprintf(stderr, "parsedecl\n");
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
	fprintf(stderr, "parsearray\n");
	if (token->type != SIGN_BROP)
		return;
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
	Node *np;

	np = NULL;
	do {
		fprintf(stderr, "parsestatements\n");
		parsestatement();
		nexttoken();
		if (token->type != SIGN_TERM)
			parseerror("Expected ;");
	} while (1);
	return np;
}

static
Node *
parsestatement(void)
{
	Node *np;

	np = NULL;
	fprintf(stderr, "parsestatement\n");
	#pragma GCC diagnostic ignored "-Wswitch"
	switch (token->type) {
	case IDENTIFIER:
		nexttoken();
		parseindex();
		nexttoken();
		if (token->type != SIGN_EQUAL)
			parseerror("Expected =");
		nexttoken();
		parseexp();
		break;
	case PRINT:
		nexttoken();
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		break;
	case READ:
		nexttoken();
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		break;
	case SIGN_CBOP:
		nexttoken();
		if (token->type != SIGN_BROP)
			parseerror("Expected {");
		parsestatements();
		if (token->type != SIGN_BRCL)
			parseerror("Expected }");
		break;
	case IF:
		nexttoken();
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		parsestatement();
		if (token->type != ELSE)
			parseerror("Expected else");
		break;
	case WHILE:
		nexttoken();
		if (token->type != WHILE)
			parseerror("Expected while");
		if (token->type != SIGN_PAROP)
			parseerror("Expected (");
		parseexp();
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected )");
		nexttoken();
		parsestatement();
		break;
	default:
		parseerror("Expected Identifier, if, while or }");
	}
	return np;
}

static
Node *
parseindex(void)
{
	Node *np;

	np = NULL;
	fprintf(stderr, "parseindex\n");
	if (token->type != SIGN_BROP)
		return;
	nexttoken();
	parseexp();
	nexttoken();
	if (token->type != SIGN_BRCL)
		parseerror("Expected ]");
	return np;
}

static
Node *
parseexp(void)
{
	Node *np;

	np = NULL;
	fprintf(stderr, "parseexp\n");
	parseexp2();
	nexttoken();
	parseop_exp();
	return np;
}

static
Node *
parseexp2(void)
{
	Node *np;

	np = NULL;
	fprintf(stderr, "parseexp2\n");
	switch (token->type) {
	case SIGN_PAROP:
		nexttoken();
		if (token->type != SIGN_PARCL)
			parseerror("Expected (");
		break;
	case IDENTIFIER:
		nexttoken();
		parseindex();
		break;
	case INTEGER:
		break;
	case SIGN_MINUS:
		nexttoken();
		parseexp2();
		break;
	case SIGN_NOT:
		nexttoken();
		parseexp2();
		break;
	}
	return np;
}

static
Node *
parseop_exp(void)
{
	Node *np;

	np = NULL;
	fprintf(stderr, "parseop_exp\n");
	parseop();
	nexttoken();
	parseexp();
	return np;
}

static
Node *
parseop(void)
{
	Node *np;

	fprintf(stderr, "parseop\n");
	np = makenode();
	switch (token->type) {
	case SIGN_PLUS:
		break;
	case SIGN_MINUS:
		break;
	case SIGN_MULT:
		break;
	case SIGN_DIV:
		break;
	case SIGN_LESS:
		break;
	case SIGN_GRTR:
		break;
	case SIGN_EQUAL:
		break;
	case SIGN_UNEQL:
		break;
	case SIGN_AND:
		break;
	default:
		free(np);
		return NULL;
	}
	return np;
}
