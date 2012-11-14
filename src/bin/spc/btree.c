#include "error.h"
#include "spc.h"

Node *
makenode(void)
{
	Node *node;

	if((node = malloc(sizeof(Node))) == NULL)
		die("Failed to allocate memory for parse tree node.")
	node->left = NULL;
	node->right = NULL;
	node->data.sym = NULL;
}
