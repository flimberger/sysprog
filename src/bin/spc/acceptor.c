#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "error.h"
#include "spc.h"
#include "strtab.h"
#include "symtab.h"

enum {
	LEXLEN = 1024
};

typedef enum {
	NWLEX,
	IDENT,
	INTEG,
	OPADD,	/* + */
	OPSUB,	/* - */
	OPDIV,	/* / */
	OPMUL,	/* * */
	OLESS,	/* < */
	OGRTR,	/* > */
	OASGN,	/* = */
	OUNEQ,	/* <!> */
	OPNOT,	/* ! */
	OPAND,	/* & */
	OTERM,	/* ; */
	OPAOP,	/* ( */
	OPACL,	/* ) */
	OCBOP,	/* { */
	OCBCL,	/* } */
	OBROP,	/* [ */
	OBRCL,	/* ] */
	CMMNT,
	CMEND,
	ENDTK,
	NXTTK,
	ERTOK,
	ENDLX
} State;

extern Strtab *strtab;
extern Symbol **symtab;

static char buf[LEXLEN], c;
static size_t col, i, tkcol, tkrow, row;
static Buffer *src, *out;
static State last;

static void
printtoken(Token *tk)
{
	if (tk == NULL)
		return;
	switch (tk->type) {
	case ERROR:
		bprintf(out, "ERROR at char %d line %d\n", tk->col, tk->row);
		break;
	case IDENTIFIER:
		bprintf(out, "Token IDENTIFIER char %d line %d Lexem %s\n", tk->col, tk->row, tk->data.sym->info->lexem);
		break;
	case INTEGER:
		bprintf(out, "Token INTEGER char %d line %d Value %ld\n", tk->col, tk->row, tk->data.val);
		break;
	case PRINT:
		bprintf(out, "Token PRINT char %d line %d Lexem %s\n", tk->col, tk->row, tk->data.sym);
		break;
	case READ:
		bprintf(out, "Token READ char %d line %d Lexem %s\n", tk->col, tk->row, tk->data.sym);
		break;
	case SIGN:
		bprintf(out, "Token SIGN char %d line %d Sign %s\n", tk->col, tk->row, tk->data.sign);
		break;
	}
}

static void
maketoken(void)
{
	Token tk;
	const char *lex;

	tk.row = tkrow;
	tk.col = tkcol;
	switch (last) {
	case IDENT:
		tk.type = IDENTIFIER;
		lex = strtab_insert(strtab, buf);
		tk.data.sym = storesym(symtab, lex);
		break;
	case INTEG:
		tk.type = INTEGER;
		tk.data.val = atol(buf);
		break;
	case OPADD:	/* + */
		tk.data.sign = "+";
	case OPSUB:	/* - */
		tk.data.sign = "-";
	case OPDIV:	/* / */
		tk.data.sign = "/";
	case OPMUL:	/* * */
		tk.data.sign = "*";
	case OLESS:	/* < */
		tk.data.sign = "<";
	case OGRTR:	/* > */
		tk.data.sign = ">";
	case OASGN:	/* = */
		tk.data.sign = "=";
	case OUNEQ:	/* <!> */
		tk.data.sign = "<!>";
	case OPNOT:	/* ! */
		tk.data.sign = "!";
	case OPAND:	/* & */
		tk.data.sign = "&";
	case OTERM:	/* ; */
		tk.data.sign = ";";
	case OPAOP:	/* ( */
		tk.data.sign = "(";
	case OPACL:	/* ) */
		tk.data.sign = ")";
	case OCBOP:	/* { */
		tk.data.sign = "{";
	case OCBCL:	/* } */
		tk.data.sign = "}";
	case OBROP:	/* [ */
		tk.data.sign = "[";
	case OBRCL:	/* ] */
		tk.data.sign = "]";
		tk.type = SIGN;
		break;
	case CMMNT:
	case CMEND:
		bprintf(out, "ERTOK: comments should be ignored!\n");
	case ENDTK:
	case NXTTK:
	case ERTOK:
		tk.type = ERROR;
		break;
	case ENDLX:
	case NWLEX:
	default:
		bprintf(out, "not yet handled\n");
		tk.type = ERROR;
	}
	printtoken(&tk);
}

static int
getchar(void)
{
	if ((c = bgetchar(src)) == EOF) {
		if (errno != 0)
			die(1, "read error:");
	} else {
		if (i > LEXLEN)
			die(2, "lexem too long");
		buf[i++] = c;
		switch (c) {
		case '\n':
			row++;
			col = 0;
			break;
		default:
			col++;
		}
	}
	return c;
}

static State
nwlex(void)
{
	if ((c = getchar()) == EOF)
		return ENDLX;
	if (isspace(c)) {
		i--;
		return NWLEX;
	}
	tkrow = row;
	tkcol = col;
	if (isalpha(c))
		return IDENT;
	if (isdigit(c))
		return INTEG;
	switch (c) {
	case '+':
		return OPADD;
	case '-':
		return OPSUB;
	case '/':
		return OPDIV;
	case '*':
		return OPMUL;
	case '<':
		return OLESS;
	case '>':
		return OGRTR;
	case '=':
		return OASGN;
	case '!':
		return OPNOT;
	case '&':
		return OPAND;
	case ';':
		return OTERM;
	case '(':
		return OPAOP;
	case ':':
		return OPACL;
	case '{':
		return OCBOP;
	case '}':
		return OCBCL;
	case '[':
		return OBROP;
	case ']':
		return OBRCL;
	default:
		return ERTOK;
	}
}

static State
ident(void)
{
	last = IDENT;
	if ((c = getchar()) == EOF)
		return ENDLX;
	if (isalnum(c))
		return IDENT;
	else if (isspace(c))
		return ENDTK;
	return NXTTK;
}

static State
integ(void)
{
	last = INTEG;
	if ((c = getchar()) == EOF)
		return ENDLX;
	if (isdigit(c))
		return INTEG;
	else if (isspace(c))
		return ENDTK;
	return NXTTK;
}

static State
opadd(void)
{
	last = OPADD;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opsub(void)
{
	last = OPSUB;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opdiv(void)
{
	last = OPDIV;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opmul(void)
{
	last = OPMUL;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
oless(void)
{
	last = OLESS;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
ogrtr(void)
{
	last = OGRTR;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
oasgn(void)
{
	last = OASGN;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
ouneq(void)
{
	last = OUNEQ;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opnot(void)
{
	last = OPNOT;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opand(void)
{
	last = OPAND;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
oterm(void)
{
	last = OTERM;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opaop(void)
{
	last = OPAOP;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
opacl(void)
{
	last = OPACL;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
ocbop(void)
{
	last = OCBOP;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
ocbcl(void)
{
	last = OCBCL;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
obrop(void)
{
	last = OBROP;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
obrcl(void)
{
	last = OBRCL;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
cmmnt(void)
{
	last = CMMNT;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
cmend(void)
{
	last = CMEND;
	if ((c = getchar()) == EOF)
		return ENDLX;
	return NXTTK;
}

static State
endtk(void)
{
	if ((c = getchar()) != EOF) {
		buf[--i] = '\0';
		maketoken();
		i = 0;
		return NWLEX;
	}
	return ERTOK;
}

static State
nxttk(void)
{
	buf[--i] = '\0';
	bungetchar(src);
	maketoken();
	i = 0;
	return NWLEX;
}

static State
error(void)
{
	last = ERTOK;
	maketoken();
	return ENDLX;
}

State (*acceptor[])(void) = {
	nwlex,
	ident,
	integ,
	opadd,
	opsub,
	opdiv,
	opmul,
	oless,
	ogrtr,
	oasgn,
	ouneq,
	opnot,
	opand,
	oterm,
	opaop,
	opacl,
	ocbop,
	ocbcl,
	obrop,
	obrcl,
	cmmnt,
	cmend,
	endtk,
	nxttk,
	error
};

void
lex(Buffer *restrict ibuf, Buffer *restrict obuf)
{
	State s;

	src = ibuf;
	out = obuf;
	row = 1;
	s = NWLEX;
	while ((s = (*acceptor[s])()) != ENDLX)
		;
}
