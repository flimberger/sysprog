#ifndef _SYSPROG_H_
#define _SYSPROG_H_

#include "buffer.h"
#include "defs.h"
#include "symtab.h"

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

const char *const tokennames[] = {
	"END",
	"ERROR",
	"IDENTIFIER",
	"INTEGER",
	"PRINT",
	"READ",
	"SIGN_PLUS",
	"SIGN_MINUS",
	"SIGN_DIV",
	"SIGN_MULT",
	"SIGN_LESS",
	"SIGN_GRTR",
	"SIGN_EQUAL",
	"SIGN_UNEQL",
	"SIGN_NOT",
	"SIGN_AND",
	"SIGN_TERM",
	"SIGN_PAROP",
	"SIGN_PARCL",
	"SIGN_CBOP",
	"SIGN_CBCL",
	"SIGN_BROP",
	"SIGN_BRCL"
};

char *infile;
Buffer *src, *out;
Symtab *symtab;

int compile(char *outfile);
Token *gettoken(void);
void printtoken(Token *tp);
void syminit(void);
void symterm(void);

#endif /* _SYSPROG_H_ */
