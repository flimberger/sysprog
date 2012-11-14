#include <stdio.h>
#include <stdlib.h>

#include "spc.h"

static void parsedecls(void);
static void parsedecl(void);
static void parsearray(void);
static void parsestatements(void);
static void parsestatement(void);
static void parseindex(void);
static void parseexp(void);
static void parseexp2(void);
static void parseop_exp(void);
static void parseop(void);

static Token *token;

static inline
void
parseerror(const char *const str)
{
	fprintf(stderr, "%s:%u:%u: Error on Token %s: %s\n", infile, token->row,
			token->col, tokennames[token->type], str);
	exit(EXIT_FAILURE);
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
	nexttoken();
	parsedecls();
	parsestatements();
}

static
void
parsedecls(void)
{
	do {
		fprintf(stderr, "parsedecls\n");
		parsedecl();
		nexttoken();
		if (token->type != SIGN_TERM)
			parseerror("Expected ;");
		nexttoken();
	} while (token->type == INT);
}

static
void
parsedecl(void)
{
	fprintf(stderr, "parsedecl\n");
	if (token->type != INT)
		parseerror("Expected int keyword");
	nexttoken();
	parsearray();
	if (token->type != IDENTIFIER)
		parseerror("Expected Identifier");
}

static
void
parsearray(void)
{
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
}

static
void
parsestatements(void)
{
	do {
		fprintf(stderr, "parsestatements\n");
		parsestatement();
		nexttoken();
		if (token->type != SIGN_TERM)
			parseerror("Expected ;");
	} while (1);
}

static
void
parsestatement(void)
{
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
}

static
void
parseindex(void)
{
	fprintf(stderr, "parseindex\n");
	if (token->type != SIGN_BROP)
		return;
	nexttoken();
	parseexp();
	nexttoken();
	if (token->type != SIGN_BRCL)
		parseerror("Expected ]");
}

static
void
parseexp(void)
{
	fprintf(stderr, "parseexp\n");
	parseexp2();
	nexttoken();
	parseop_exp();
}

static
void
parseexp2(void)
{
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
}

static
void
parseop_exp(void)
{
	fprintf(stderr, "parseop_exp\n");
	parseop();
	nexttoken();
	parseexp();
}

static
void
parseop(void)
{
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
	}
}
