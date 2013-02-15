#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "spc.h"

const char *nodenames[] = {
	"NODE_NONE",
	/* nonterminal symbols */
	"NODE_PROG",
	"NODE_DECLS",
	"NODE_DECL",
	"NODE_ARRAY",
	"NODE_STATEMENTS",
	"NODE_STATEMENT",
	"NODE_EXP",
	"NODE_EXP2",
	"NODE_INDEX",
	"NODE_OPEXP",
	"NODE_OP",
	/* terminal symbols */
	"NODE_PRINT",
	"NODE_READ",
	"NODE_IF",
	"NODE_WHILE",
	"NODE_IDENT",
	"NODE_INTCONST"
};

Node *
makenode(Nodetype type, uint row, uint col)
{
	Node *node;

	if((node = malloc(sizeof(Node))) == NULL)
		die(EXIT_FAILURE, "Failed to allocate memory for parse tree node.");
	node->left = NULL;
	node->right = NULL;
	node->data.sym = NULL;
	node->type = type;
	node->col = col;
	node->row = row;
	return node;
}


void
printnode(Node *node)
{
	fprintf(stderr, "type: %s; addr: %p; left: %p; right: %p\n", nodenames[node->type], (void *) node, (void *) node->left, (void *) node->right);
}
