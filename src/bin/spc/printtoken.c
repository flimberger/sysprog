#include <stdio.h>

#include "defs.h"
#include "spc.h"
#include "error.h"

void
printtoken(Token *tp)
{
	if (tp == NULL)
		return;
	switch (tp->symtype) {
	case S_NONE:
		fprintf(stderr, "Token %-10s char %4d line %3d Char  %c\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.lastchar);
		break;
	case S_END:
		break;
	case S_ERROR:
		fprintf(stderr, "Token %-10s char %4d line %3d Char  %c\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.lastchar);
		break;
	case S_INTCONST:
		fprintf(stderr, "Token %-10s char %4d line %3d Value %lld\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.val);
		break;
	case S_ELSE:
	case S_IF:
	case S_INT:
	case S_IDENT:
	case S_PRINT:
	case S_READ:
	case S_WHILE:
		fprintf(stderr, "Token %-10s char %4d line %3d Lexem %s\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.sym->lexem);
		break;
	case S_PLUS:
	case S_MINUS:
	case S_DIV:
	case S_MULT:
	case S_LESS:
	case S_GRTR:
	case S_EQUAL:
	case S_UNEQL:
	case S_NOT:
	case S_AND:
	case S_TERM:
	case S_PAROP:
	case S_PARCL:
	case S_CBOP:
	case S_CBCL:
	case S_BROP:
	case S_BRCL:
		fprintf(stderr, "Token %-10s char %4d line %3d Sign  %s\n", tokennames[tp->symtype], tp->col, tp->row, tp->data.sign);
		break;
	default:
		die(3, "Unknown Token %s on char %4d line %3d\n", tokennames[tp->symtype], tp->col, tp->row);
	}
}
