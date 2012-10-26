#include <ctype.h>
#include <errno.h>
#include <stdlib.h>

#include "error.h"
#include "symtab.h"

#include "spc.h"

enum {
	LEXLEN = 1024 * 4
};

typedef enum {
	LXNEW,
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
	CUNEQ,
	LXERR,
	MKTOK,
	LXEOF,
	LXEND
} State;

static char buf[LEXLEN], c;
static size_t col, i, tkcol, tkrow, row = 1;
static State last;
static Token token;

static int
getchar(void)
{
	if ((c = bgetchar(src)) == EOF) {
		if (errno != 0)
			die(1, "read error:");
	} else {
		if (i >= LEXLEN)
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

static void
ungetchar(void)
{
	if (c == '\n')
		row--;
	else
		col--;
	bungetchar(src);
}

static State
nwlex(void)
{
	last = LXNEW;
	if ((c = getchar()) == EOF)
		return LXEOF;
	if (isspace(c)) {
		i--;
		return LXNEW;
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
	case ')':
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
		return LXERR;
	}
}

static State
ident(void)
{
	last = IDENT;
	if ((c = getchar()) == EOF)
		return MKTOK;
	if (isalnum(c))
		return IDENT;
	else if (isspace(c))
		return MKTOK;
	ungetchar();
	return MKTOK;
}

static State
integ(void)
{
	last = INTEG;
	if ((c = getchar()) == EOF)
		return MKTOK;
	if (isdigit(c))
		return INTEG;
	else if (isspace(c))
		return MKTOK;
	ungetchar();
	return MKTOK;
}

static State
opadd(void)
{
	last = OPADD;
	return MKTOK;
}

static State
opsub(void)
{
	last = OPSUB;
	return MKTOK;
}

static State
opdiv(void)
{
	last = OPDIV;
	if ((c = getchar()) == EOF)
		return MKTOK;
	if (c == '*')
		return CMMNT;
	ungetchar();
	return MKTOK;
}

static State
opmul(void)
{
	last = OPMUL;
	return MKTOK;
}

static State
oless(void)
{
	last = OLESS;
	if ((c = getchar()) == EOF)
		return MKTOK;
	if (c == '!')
		return CUNEQ;
	ungetchar();
	return MKTOK;
}

static State
ogrtr(void)
{
	last = OGRTR;
	return MKTOK;
}

static State
oasgn(void)
{
	last = OASGN;
	return MKTOK;
}

static State
ouneq(void)
{
	last = OUNEQ;
	return MKTOK;
}

static State
opnot(void)
{
	last = OPNOT;
	return MKTOK;
}

static State
opand(void)
{
	last = OPAND;
	return MKTOK;
}

static State
oterm(void)
{
	last = OTERM;
	return MKTOK;
}

static State
opaop(void)
{
	last = OPAOP;
	return MKTOK;
}

static State
opacl(void)
{
	last = OPACL;
	return MKTOK;
}

static State
ocbop(void)
{
	last = OCBOP;
	return MKTOK;
}

static State
ocbcl(void)
{
	last = OCBCL;
	return MKTOK;
}

static State
obrop(void)
{
	last = OBROP;
	return MKTOK;
}

static State
obrcl(void)
{
	last = OBRCL;
	return MKTOK;
}

static State
cmmnt(void)
{
	last = CMMNT;
	if ((c = getchar()) == EOF)
		return LXEOF;
	if (c == '*')
		return CMEND;
	return CMMNT;
}

static State
cmend(void)
{
	last = CMEND;
	if ((c = getchar()) == EOF)
		return LXEOF;
	if (c == '/') {
		i = 0;
		return LXNEW;
	}
	return CMMNT;
}

static State
cuneq(void)
{
	last = CUNEQ;
	if ((c = getchar()) != '>') {
		ungetchar();
		ungetchar();
		last = OLESS;
		return MKTOK;
	}
	return OUNEQ;
}

static State
lxerr(void)
{
	last = LXERR;
	return MKTOK;
}

static State
mktok(void)
{
	buf[i - 1] = '\0';
	i = 0;
	token.row = tkrow;
	token.col = tkcol;
	switch (last) {
	case IDENT:
		token.data.sym = storesym(symtab, buf);
		if (token.data.sym->type == 0)
			token.data.sym->type = IDENTIFIER;
		token.type = token.data.sym->type;
		break;
	case INTEG:
		token.type = INTEGER;
		token.data.val = atol(buf);
		break;
	case OPADD:	/* + */
		token.data.sign = "+";
		token.type = SIGN_PLUS;
		break;
	case OPSUB:	/* - */
		token.data.sign = "-";
		token.type = SIGN_MINUS;
		break;
	case OPDIV:	/* / */
		token.data.sign = "/";
		token.type = SIGN_DIV;
		break;
	case OPMUL:	/* * */
		token.data.sign = "*";
		token.type = SIGN_MULT;
		break;
	case OLESS:	/* < */
		token.data.sign = "<";
		token.type = SIGN_LESS;
		break;
	case OGRTR:	/* > */
		token.data.sign = ">";
		token.type = SIGN_GRTR;
		break;
	case OASGN:	/* = */
		token.data.sign = "=";
		token.type = SIGN_EQUAL;
		break;
	case OUNEQ:	/* <!> */
		token.data.sign = "<!>";
		token.type = SIGN_UNEQL;
		break;
	case OPNOT:	/* ! */
		token.data.sign = "!";
		token.type = SIGN_NOT;
		break;
	case OPAND:	/* & */
		token.data.sign = "&";
		token.type = SIGN_AND;
		break;
	case OTERM:	/* ; */
		token.data.sign = ";";
		token.type = SIGN_TERM;
		break;
	case OPAOP:	/* ( */
		token.data.sign = "(";
		token.type = SIGN_PAROP;
		break;
	case OPACL:	/* ) */
		token.data.sign = ")";
		token.type = SIGN_PARCL;
		break;
	case OCBOP:	/* { */
		token.data.sign = "{";
		token.type = SIGN_CBOP;
		break;
	case OCBCL:	/* } */
		token.data.sign = "}";
		token.type = SIGN_CBCL;
		break;
	case OBROP:	/* [ */
		token.data.sign = "[";
		token.type = SIGN_BROP;
		break;
	case OBRCL:	/* ] */
		token.data.sign = "]";
		token.type = SIGN_BRCL;
		break;
	case LXERR:
		token.data.lastchar = c;
		token.type = ERROR;
		break;
	case LXEOF:
		token.type = END;
		break;
	default:
		die(1, "acceptor ended up in an unexpected state");
	}
	return LXEND;
}

static State
lxeof(void)
{
	last = LXEOF;
	return MKTOK;
}

static State
lxend(void)
{
	die(1, "acceptor tried to execute LXEND");
	return LXEND;
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
	cuneq,
	lxerr,
	mktok,
	lxeof,
	lxend
};

Token *
gettoken(void)
{
	State s;

	s = LXNEW;
	while ((s = (*acceptor[s])()) != LXEND)
		;
	return &token;
}
