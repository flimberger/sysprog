#include <stdio.h>
#include <stdlib.h>

#include "spc.h"

static void parsedecls(void);
static void parsedecl(void);
static void parsearray(void);
static void parsestatements(void);

static Token *token;

static inline
void
parseerror(const char *const str)
{
	fprintf(stderr, "%s:%u: Error on Token %s: %s\n", infile, token->row,
			tokennames[token->type], str);
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
parsedecls()
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
	fprintf(stderr, "parsestatements\n");
	switch (token->type) {
	case IDENTIFIER:
	case IF:
	case SIGN_CBOP:
	case WHILE:
		return;
	default:
		parseerror("Expected Identifier, if, while or }");
	}
}
