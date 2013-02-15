#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "spc.h"

static void checkdecls(Node *node);
static void checkdecl(Node *node);
static void checkarray(Node *node);
static void checkstatements(Node *node);
static void checkstatement(Node *node);
static void checkindex(Node *node);
static void checkexp(Node *node);
static void checkexp2(Node *node);
static void checkop_exp(Node *node);
static void checkop(Node *node);

void
checkprog(Node *node)
{
	if (node == NULL)
		die(EXIT_FAILURE, "Unexpected Nullpointer in checkprog");

	if (node->type != NODE_PROG)
		die(EXIT_FAILURE, "Root Node expected in checkprog");

	checkdecls(node->left);
	checkstatements(node->right);
	node->datatype = T_NONE;
}

static
void
checkdecls(Node *node)
{
	if (node == NULL)
		return;

	if (node->type != NODE_DECLS)
		die(EXIT_FAILURE, "Expected NODE_DECLS in checkdecls");
	checkdecl(node->left);
	checkdecls(node->right);
	node->datatype = T_NONE;
}

static
void
checkdecl(Node *node)
{
	if (node == NULL)
		panic("Unexpected Nullpointer in checkdecl");

	if (node->datatype != T_NONE) {
		warn("Identifier %s already definied", node->data.sym->lexem);
		node->datatype = node->data.sym->datatype = T_ERROR;
	} else if (node->left == NULL) {
		node->datatype = node->data.sym->datatype = T_INT;
	} else {
		checkarray(node->left);
		if (node->left->datatype == T_ERROR) {
			node->datatype = node->data.sym->datatype = T_ERROR;
		} else {
			node->datatype = node->data.sym->datatype = T_INTARR;
		}
	}
}

static
void
checkarray(Node *node)
{
	if (node == NULL)
		panic("Unexpected Nullpointer in checkarray");

	if (node->data.val > 0) {
		node->datatype = T_ARRAY;
	} else {
		warn("%lu is not a valid dimension", node->data.val);
		node->datatype = T_ERROR;
	}
}

static
void
checkstatements(Node *node)
{
	if (node == NULL)
		return;

	checkstatement(node->left);
	checkstatements(node->right);
	node->datatype = T_NONE;
}

static
void
checkstatement(Node *node)
{
	if (node == NULL)
		panic("Unexpected Nullpointer in checkstatement");

	fprintf(stderr, "%d\n", node->left->type);
	switch (node->left->type) {
	case NODE_IDENT:
		checkindex(node->right->left);
		checkexp(node->right->right);
		if (node->left->data.sym->datatype == T_NONE) {
			warn("Identifier %s not definied", node->left->data.sym->lexem);
		} else if ((node->right->right->datatype == T_INT) &&
					(((node->left->data.sym->datatype == T_INT) && (node->right->left == NULL)) ||
					((node->left->data.sym->datatype == T_INTARR) && (node->right->left != NULL)
						&& node->right->left->datatype == T_ARRAY))) {
			node->datatype = T_NONE;
		} else {
			node->datatype = T_ERROR;
			warn("Incompatible types");
		}
		break;
	case NODE_PRINT:
		checkexp(node->left->left);
		node->datatype = T_NONE;
		break;
	case NODE_READ:
		checkindex(node->left->right);
		if (node->left->left->data.sym->datatype == T_NONE) {
			warn("Identifier %s not definied", node->left->left->data.sym->lexem);
		} else if (((node->left->left->data.sym->datatype == T_INT) && (node->left->right == NULL)) ||
					((node->left->left->data.sym->datatype == T_INTARR) && (node->left->right != NULL)
						&& node->left->right->datatype == T_ARRAY)) {
			node->datatype = T_NONE;
		} else {
			node->datatype = T_ERROR;
			warn("Incompatible types");
		}
		break;
	case NODE_STATEMENTS:
		checkstatements(node);
		break;
	case NODE_IF:
		checkexp(node->right);
		checkstatement(node->left->left); /* IF */
		checkstatement(node->left->right); /* ELSE */
		if (node->right->datatype == T_ERROR)
			node->datatype = T_ERROR;
		else
			node->datatype = T_NONE;
		break;
	case NODE_WHILE:
		checkexp(node->right);
		checkstatement(node->left->left);
		if (node->right->datatype == T_ERROR)
			node->datatype = T_ERROR;
		else
			node->datatype = T_NONE;
		break;
	default:
		warn("Unexpected node type in checkstatement().");
	}
}

static
void
checkindex(Node *node)
{
	if (node == NULL)
		return;

	checkexp(node->left);
	if (node->left->datatype == T_ERROR)
		node->datatype = T_ERROR;
	else
		node->datatype = T_NONE;
}

static
void
checkexp(Node *node)
{
	if (node == NULL)
		panic("Unexpected Nullpointer in checkexp");

	fprintf(stderr, "%d\n", node->type);
	fprintf(stderr, "%p\n", (void*)node->left);
	fprintf(stderr, "%p\n", (void*)node->right);
	fprintf(stderr, "%s\n", node->data.sym->lexem);
	checkexp2(node->left);
	checkop_exp(node->right);

	if (node->right == NULL)
		node->datatype = node->left->datatype;
	else if (node->left->datatype != node->right->datatype)
		node->datatype = T_ERROR;
	else
		node->datatype = node->left->datatype;
}

static
void
checkexp2(Node *node)
{
	if (node == NULL)
		panic("Unexpected Nullpointer in checkexp2");

	switch (node->left->type) {
	case NODE_EXP:
		checkexp(node->left);
		node->datatype = node->left->datatype;
		break;
	case NODE_IDENT:
		if (node->left->data.sym->datatype == T_NONE) {
			node->datatype = T_ERROR;
			warn("Identifier %s not definied", node->left->data.sym->lexem);
		} else if (((node->left->datatype == T_INT) && (node->right == NULL)) ||
					((node->left->datatype == T_INTARR) && (node->right != NULL)
						&& (node->right->datatype == T_ARRAY))) {
			node->datatype = T_INT; /* Not expandable, but less special cases */
		} else {
			node->datatype = T_ERROR;
			warn("No primitive type");			
		}
		break;
	case NODE_INTCONST:
		node->datatype = T_INT;
		break;
	case NODE_OP:
		checkexp2(node->right);
		if (node->left->data.op == OP_NOT) {
			if (node->right->datatype != T_INT)
				node->datatype = T_ERROR;
			else
				node->datatype = T_INT;	
		} else {
			node->datatype = node->right->datatype;	
		}
		break;
	default:
		warn("Unexpected node type in checkexp2().");
	}
}

static
void
checkop_exp(Node *node)
{
	if (node == NULL)
		return;

	checkop(node->left);
	checkexp(node->right);
	node->datatype = node->right->datatype;
}

static
void
checkop(Node *node)
{
	if (node == NULL)
		panic("Unexpected Nullpointer in checkop");

	node->datatype = T_NONE;
}