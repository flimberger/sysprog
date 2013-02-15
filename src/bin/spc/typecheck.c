#include <stdlib.h>

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
checkprog(Node *node) {
	if (node == NULL)
		die(EXIT_FAILURE, "Unexpected Nullpointer in checkprog");

	if (node->type != NODE_ROOT)
		die(EXIT_FAILURE, "Root Node expected in checkprog");

	checkdecls(node->left);
	checkstatements(node->right);
	node->datatype = T_NONE;
}

static
void
checkdecls(Node *node) {
	if (node == NULL)
		return;

	if (node->type == NODE_LIST) {
		checkdecl(node->left);
		checkdecls(node->right);
		node->datatype = T_NONE;		
	} else if (node->type == NODE_DECL) {
		checkdecl(node);
	} else {
		die(EXIT_FAILURE, "Expected NODE_LIST in checkdecls");		
	}	
}

static
void
checkdecl(Node *node) {
	if (node == NULL)
		die(EXIT_FAILURE, "Unexpected Nullpointer in checkdecl");

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
checkarray(Node *node) {
	if (node == NULL)
		die(EXIT_FAILURE, "Unexpected Nullpointer in checkarray");

	if (node->data.val > 0) {
		node->datatype = T_ARRAY;
	} else {
		warn("%lu is not a valid dimension", node->data.val);
		node->datatype = T_ERROR;
	}
}

static
void
checkstatements(Node *node) {
	if (node == NULL)
		return;

	checkstatement(node->left);
	checkstatements(node->right);
	node->datatype = T_NONE;
}

static
void
checkstatement(Node *node) {
	if (node == NULL)
		die(EXIT_FAILURE, "Unexpected Nullpointer in checkstatement");

	switch (node->type) {
	case NODE_ASSGN:
		checkindex(node->left);
		checkexp(node->right);
		if (node->data.sym->datatype == T_NONE) {
			warn("Identifier %s not definied", node->data.sym->lexem);
		} else if ((node->right->datatype == T_INT) &&
					(((node->data.sym->datatype == T_INT) && (node->left == NULL)) ||
					((node->data.sym->datatype == T_INTARR) && (node->left != NULL)
						&& node->left->datatype == T_ARRAY))) {
			node->datatype = T_NONE;
		} else {
			node->datatype = T_ERROR;
			warn("Incompatible types");
		}
		break;
	case NODE_PRINT:
		checkexp(node->left);
		node->datatype = T_NONE;
		break;
	case NODE_READ:
		checkindex(node->left->right);
		if (node->data.sym->datatype == T_NONE) {
			warn("Identifier %s not definied", node->data.sym->lexem);
		} else if (((node->left->data.sym->datatype == T_INT) && (node->left->right == NULL)) ||
					((node->left->data.sym->datatype == T_INTARR) && (node->left->right != NULL)
						&& node->left->right->datatype == T_ARRAY)) {
			node->datatype = T_NONE;
		} else {
			node->datatype = T_ERROR;
			warn("Incompatible types");
		}
		break;
	case NODE_IF:
		checkexp(node->left);
		checkstatement(node->right->left); /* IF */
		checkstatement(node->right->right); /* ELSE */
		if (node->left->datatype == T_ERROR)
			node->datatype = T_ERROR;
		else
			node->datatype = T_NONE;
		break;
	case NODE_WHILE:
		checkexp(node->left);
		checkstatement(node->right);
		if (node->left->datatype == T_ERROR)
			node->datatype = T_ERROR;
		else
			node->datatype = T_NONE;
		break;
	default:
		if (node->type == NODE_LIST)
			checkstatements(node);
		else /* Prematurely optimized tree */
			checkstatement(node);
	}
}

static
void
checkindex(Node *node) {
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
checkexp(Node *node) {
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
checkexp2(Node *node) {
	switch (node->left->type) {
	case NODE_IDENT:
		if (node->left->data.sym->datatype == T_NONE) {
			node->datatype = T_ERROR;
			warn("Identifier %s not definied", node->left->data.sym->lexem);
		} else if (((node->left->datatype == T_INT) && (node->right == NULL)) ||
					(node->left->datatype == T_INTARR && node->right->datatype == T_ARRAY)) {
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
		checkexp(node->left);
		node->datatype = node->left->datatype;
	}
}

static
void
checkop_exp(Node *node) {
	if (node == NULL)
		return;

	checkop(node->left);
	checkexp(node->right);
	node->datatype = node->right->datatype;
}

static
void
checkop(Node *node) {
	node->datatype = T_NONE;
}