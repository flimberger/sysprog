#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "spc.h"

Node *
makenode(Nodetype type)
{
	Node *node;

	if((node = malloc(sizeof(Node))) == NULL)
		die(EXIT_FAILURE, "Failed to allocate memory for parse tree node.");
	node->left = NULL;
	node->right = NULL;
	node->data.sym = NULL;
	node->type = type;
	fprintf(stderr, "%d\n", type);
	return node;
}
