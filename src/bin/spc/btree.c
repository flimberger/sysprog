#include "spc.h"

Node *
makenode(void)
{
	Node *node;

	if((node = malloc(sizeof(Node))) == NULL)
		return NULL;
	node->left = NULL;
	node->right = NULL;
	node->data.sym = NULL;
}
