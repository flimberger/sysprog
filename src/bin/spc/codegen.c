#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "error.h"
#include "spc.h"
#include "spvm_opcodes.h"

#define LBLLEN	32

static void gendecls(Node *node);
static void gendecl(Node *node);
static void genarray(Node *node);
static void genstatements(Node *node);
static void genstatement(Node *node);
static void genindex(Node *node);
static void genexp(Node *node);
static void genexp2(Node *node);
static void genop_exp(Node *node);
static void genop(Node *node);

static
char *
newlabel()
{
	static uvlong c;
	char *l;

	l = calloc(LBLLEN, sizeof(char));
	snprintf(l, LBLLEN, "l%llu", c++);
	return l;
}

void
genprog(Node *node)
{
	gendecls(node->left);
	genstatements(node->right);
	bprintf(out, "%s\n", spvm_op[STP]);
}

static
void
gendecls(Node *node)
{
	if (node == NULL)
		return;

	if (node->type != NODE_DECLS)
		die(EXIT_FAILURE, "Unexpected nodetype in gendecls().");

	gendecl(node->left);
	gendecls(node->right);		
}

static
void
gendecl(Node *node)
{
	if (node == NULL)
		panic("Unexpected nullpointer in gendecl().");
	bprintf(out, "%s $%s ", spvm_op[DS], node->data.sym->lexem);
	genarray(node->left);
}

static
void
genarray(Node *node)
{
	if (node == NULL)
		bprintf(out, "1\n");
	else
		bprintf(out, "%lu\n", node->data.val);
}

static
void
genstatements(Node *node)
{
	/*
	 * Fuck the specs, who needs NOPs?
	 * if (node == NULL)
	 *	bprintf(out, "%s\n", spvm_op[NOP]);
	 */
	if ((node == NULL) || (node->left == NULL))
		return;
	genstatement(node->left);
	genstatements(node->right);
}

static
void
genstatement(Node *node)
{
	char *lbl1, *lbl2;

	if (node == NULL)
		panic("Unexpected nullpointer in genstatement().");
	switch(node->left->type) {
	case NODE_IDENT:
		genexp(node->right->right);
		bprintf(out, "%s $%s\n", spvm_op[LA], node->left->data.sym->lexem);
		genindex(node->right->left);
		bprintf(out, "%s\n", spvm_op[STR]);
		break;
	case NODE_PRINT:
		genexp(node->left->left);
		bprintf(out, "%s\n", spvm_op[PRI]);
		break;
	case NODE_READ:
		bprintf(out, "%s\n%s $%s\n", spvm_op[REA], spvm_op[LA],
		        node->left->left->data.sym->lexem);
		genindex(node->left->right);
		bprintf(out, "%s\n", spvm_op[STR]);
		break;
	case NODE_STATEMENTS: 
		genstatements(node->left);
		break;
	case NODE_IF:
		lbl1 = newlabel();
		lbl2 = newlabel();
		genexp(node->right);
		bprintf(out, "%s #%s\n", spvm_op[JIN], lbl1);
		genstatement(node->left->left);
		bprintf(out, "%s #%s\n#%s %s\n", spvm_op[JMP], lbl2, lbl1,
		        spvm_op[NOP]);
		genstatement(node->left->right);
		bprintf(out, "#%s %s\n", lbl2, spvm_op[NOP]);
		free(lbl2);
		free(lbl1);
		break;
	case NODE_WHILE:
		lbl1 = newlabel();
		lbl2 = newlabel();
		bprintf(out, "#%s %s\n", lbl1, spvm_op[NOP]);
		genexp(node->right);
		bprintf(out, "%s #%s\n", spvm_op[JIN], lbl2);
		genstatement(node->left->left);
		bprintf(out, "%s #%s\n#%s %s\n", spvm_op[JMP], lbl1, lbl2,
		        spvm_op[NOP]);
		free(lbl2);
		free(lbl1);
		break;
	default:
		warn("Unexpected node type %s in genstatement().", nodenames[node->left->type]);
	}
}

static
void
genexp(Node *node)
{
	if (node == NULL)
		panic("Unexpected nullpointer in genexp().");
	if (node->right == NULL) {
		genexp2(node->left);
	} else if (node->right->left->data.op == OP_GRTR) { /* fugly spec */
		genop_exp(node->right);
		genexp2(node->left);
		bprintf(out, "%s\n", spvm_op[LES]);
	} else {
		genexp2(node->left);
		genop_exp(node->right);
		if (node->right->left->data.op == OP_UNEQ)
			bprintf(out, "%s\n", spvm_op[NOT]);
	}
}

static
void
genindex(Node *node)
{
	if (node == NULL)
		return;
	genexp(node->left);
	bprintf(out, "%s\n", spvm_op[ADD]);
}

static
void
genexp2(Node *node)
{
	if (node == NULL)
		panic("Unexpected nullpointer in genexp2().");
	switch (node->left->type) {
	case NODE_IDENT:
		bprintf(out, "%s $%s\n", spvm_op[LA],
		        node->left->data.sym->lexem);
		genindex(node->right);
		bprintf(out, "%s\n", spvm_op[LV]);
		break;
	case NODE_INTCONST:
		bprintf(out, "%s %lld\n", spvm_op[LC],
		        node->left->data.val);
		break;
	case NODE_OP:
		if (node->left->data.op == OP_NEG) {
			bprintf(out, "%s 0\n", spvm_op[LC]);
			genexp2(node->right);
			bprintf(out, "%s\n", spvm_op[SUB]);
		} else {
			genexp2(node->right);
			bprintf(out, "%s\n", spvm_op[NOT]);
		}
		break;
	case NODE_EXP:
		genexp(node->left);
		break;
	default:
		warn("Unexpected node type %s in genexp2().", nodenames[node->left->type]);
	}
}

static
void
genop_exp(Node *node)
{
	if (node == NULL)
		return;
	genexp(node->right);
	genop(node->left);
}

static
void
genop(Node *node)
{
	if (node == NULL)
		panic("Unexpected nullpointer in genop().");
	#pragma GCC diagnostic ignored "-Wswitch" /* I'm watching you, gcc */
	switch (node->data.op) {
	case OP_ADD:
		bprintf(out, "%s\n", spvm_op[ADD]);
		break;
	case OP_SUB:
		bprintf(out, "%s\n", spvm_op[SUB]);
		break;
	case OP_DIV:
		bprintf(out, "%s\n", spvm_op[DIV]);
		break;
	case OP_MUL:
		bprintf(out, "%s\n", spvm_op[MUL]);
		break;
	case OP_LESS:
		bprintf(out, "%s\n", spvm_op[LES]);
		break;
	case OP_GRTR:
		break;
	case OP_EQUAL:
	case OP_UNEQ:
		bprintf(out, "%s\n", spvm_op[EQU]);
		break;
	case OP_AND:
		bprintf(out, "%s\n", spvm_op[AND]);
		break;
	}
}
