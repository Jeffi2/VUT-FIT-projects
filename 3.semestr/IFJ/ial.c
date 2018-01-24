/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "ial.h"

/* ------ Auxiliary allocation check function ------ */

void alloc_check(void *ptr, char *msg)
{
	if (ptr == NULL)
	{
		fprintf(stderr, "Error at allocation of %s!\n", msg);
		exit(ERR_INTERNAL);
	}
}

/* --------- Auxiliary node stack functions --------- */

/**
 * Initialize node stack
 */
void stack_init(node_stack *stack)
{
	stack->top = -1;
	stack->arr = (bst_node **) malloc(8 * sizeof(bst_node *));
	if (stack->arr == NULL)
	{
		fprintf(stderr, "Error at allocation of node stack!\n");
		exit(ERR_INTERNAL);
	}
	stack->max = 7;
}

/**
 * Push a node pointer to the stack
 */
void stack_push(node_stack *stack, bst_node *node)
{
	if (stack->top >= stack->max)
	{
		stack->max = 2 * stack->max + 1;
		stack->arr = (bst_node **) realloc(stack->arr, (stack->max + 1) * sizeof(bst_node *));
		if (stack->arr == NULL)
		{
			fprintf(stderr, "Error at allocation of node stack!\n");
			exit(ERR_INTERNAL);
		}
	}

	stack->top += 1;
	stack->arr[stack->top] = node;
}

/**
 * Pop a node pointer from stack
 * 
 * @return   pointer from the top of the stack, if it's empty just a NULL pointer
 */
bst_node * stack_pop(node_stack *stack)
{
	if (stack->top >= 0)
	{
		return (stack->arr[stack->top--]);
	}

	return NULL;
}

/**
 * Deallocate stack and destroy it
 */
void stack_dest(node_stack *stack)
{
	stack->top = -1;
	stack->max = -1;
	free(stack->arr);
}

/* ---------- Binary search tree functions ---------- */

/**
 * Initialize tree to a empty tree
 */
void bst_init(bst_node **root)
{
	*root = NULL;
}

/**
 * Set initial values to class node
 */
void bst_class_init(bst_node *node, char *name)
{
	node->key = (char *) malloc(strlen(name) + 1);
	alloc_check((void *) node->key, "class key");
	strcpy(node->key, name);
	node->type = type_class;
	node->ptr.global = NULL;
}

/**
 * Insert a node representing a class to the tree and set initial values
 * 
 * @return   0 if succesful, SEM_ERR if the key already exists
 */
int bst_insert_class(bst_node **root, char *key)
{
	if ((*root) == NULL)
	{
		bst_node *node = (bst_node *) malloc(sizeof(bst_node));
		alloc_check((void *) node, "class node");
		bst_class_init(node, key);
		node->left = NULL;
		node->right = NULL;
		*root = node;
		return 0;
	}

	while (1)
	{
		if (strcmp((*root)->key, key) == 0)
			return SEM_ERR;

		if (strcmp((*root)->key, key) < 0)
		{
			if ((*root)->right == NULL)
			{
				bst_node *node = (bst_node *) malloc(sizeof(bst_node));
				alloc_check((void *) node, "class node");
				bst_class_init(node, key);
				node->left = NULL;
				node->right = NULL;
				(*root)->right = node;
				return 0;
			}
			else
				root = &((*root)->right);
		}
		else
		{
			if ((*root)->left == NULL)
			{
				bst_node *node = (bst_node *) malloc(sizeof(bst_node));
				alloc_check((void *) node, "class node");
				bst_class_init(node, key);
				node->left = NULL;
				node->right = NULL;
				(*root)->left = node;
				return 0;
			}
			else
				root = &((*root)->left);
		}
	}
}

/**
 * Insert node representing a variable to the tree and set initial values
 * 
 * @return   0 if succesful, SEM_ERR if the key already exists
 */
int bst_insert_variable(bst_node **root, char *key, int type)
{
	if ((*root) == NULL)
	{
		bst_node *node = (bst_node *) malloc(sizeof(bst_node));
		alloc_check((void *) node, "variable node");
		symbol_variable_init(node, key, type);
		node->left = NULL;
		node->right = NULL;
		*root = node;
		return 0;
	}

	while (1)
	{
		if (strcmp((*root)->key, key) == 0)
			return SEM_ERR;

		if (strcmp((*root)->key, key) < 0)
		{
			if ((*root)->right == NULL)
			{
				bst_node *node = (bst_node *) malloc(sizeof(bst_node));
				alloc_check((void *) node, "variable node");
				symbol_variable_init(node, key, type);
				node->left = NULL;
				node->right = NULL;
				(*root)->right = node;
				return 0;
			}
			else
				root = &((*root)->right);
		}
		else
		{
			if ((*root)->left == NULL)
			{
				bst_node *node = (bst_node *) malloc(sizeof(bst_node));
				alloc_check((void *) node, "variable node");
				symbol_variable_init(node, key, type);
				node->left = NULL;
				node->right = NULL;
				(*root)->left = node;
				return 0;
			}
			else
				root = &((*root)->left);
		}
	}
}

/**
 * Insert node representing a function to the tree and set initial values
 * 
 * @return   0 if succesful, SEM_ERR if the key already exists
 */
int bst_insert_function(bst_node **root, char *key, int type)
{
	if ((*root) == NULL)
	{
		bst_node *node = (bst_node *) malloc(sizeof(bst_node));
		alloc_check((void *) node, "function node");
		symbol_function_init(node, key, type);
		node->left = NULL;
		node->right = NULL;
		*root = node;
		return 0;
	}

	while (1)
	{
		if (strcmp((*root)->key, key) == 0)
			return SEM_ERR;

		if (strcmp((*root)->key, key) < 0)
		{
			if ((*root)->right == NULL)
			{
				bst_node *node = (bst_node *) malloc(sizeof(bst_node));
				alloc_check((void *) node, "function node");
				symbol_function_init(node, key, type);
				node->left = NULL;
				node->right = NULL;
				(*root)->right = node;
				return 0;
			}
			else
				root = &((*root)->right);
		}
		else
		{
			if ((*root)->left == NULL)
			{
				bst_node *node = (bst_node *) malloc(sizeof(bst_node));
				alloc_check((void *) node, "function node");
				symbol_function_init(node, key, type);
				node->left = NULL;
				node->right = NULL;
				(*root)->left = node;
				return 0;
			}
			else
				root = &((*root)->left);
		}
	}
}

/**
 * Add argument to list of args and to local table of function.
 *
 * @return   0 if succesful, SEM_ERR if arg with given name already exists, ERR_INTERNAL if not function
 */
int bst_node_add_arg(bst_node *node, char *name, int type)
{
	if ((node == NULL) || (node->type != type_function))
	{
		return ERR_INTERNAL;
	}
	else
	{
		if (bst_insert_variable(&(node->ptr.function->local_table), name, type) == SEM_ERR)
		{
			return SEM_ERR;
		}
		else
		{
			symbol_function_arg_add(node->ptr.function, name, type);
			return 0;
		}
	}
}

/**
 * Find a node with a given key.
 * 
 * @return   pointer on node if it's found, otherwise NULL
 */
bst_node * bst_node_find(bst_node *root, char *key)
{
	while (1)
	{
		if (root == NULL)
		{
			return NULL;
		}
		else
		{
			if (strcmp(root->key, key) == 0)
				return root;

			if (strcmp(root->key, key) < 0)
				root = root->right;
			else
				root = root->left;
		}
	}
}

/**
 * Check if variable is defined (initialized).
 *
 * @return   0 if defined, 1 if undefined, ERR_INTERNAL if node with given key doesn't exist or is not variable
 */
int bst_node_is_defined(bst_node *root, char *key)
{
	bst_node *node = bst_node_find(root, key);

	if ((node == NULL) || (node->type != type_variable))
	{
		return ERR_INTERNAL;
	}
	else
	{
		if (node->ptr.variable->init == 0)
			return 1;

		else
			return 0;
	}
}

/**
 * Make variable defined (initialized).
 */
void bst_node_define(bst_node *root, char *key)
{
	bst_node *node = bst_node_find(root, key);

	if ((node != NULL) && (node->type == type_variable))
	{
		node->ptr.variable->init = 1;
	}
}

/**
 * Copy all data except poniters from one node to another.
 * Destination node should be uninitialized.
 */
void bst_node_variable_copy(bst_node *dst, bst_node *src)
{
	symbol_variable_init(dst, src->ptr.variable->name, src->ptr.variable->type);

	if (src->ptr.variable->init == 1)
	{
		if (src->ptr.variable->type == variable_integer)
			symbol_variable_assign_int(dst->ptr.variable, src->ptr.variable->value.value_integer);

		if (src->ptr.variable->type == variable_double)
			symbol_variable_assign_double(dst->ptr.variable, src->ptr.variable->value.value_double);

		if (src->ptr.variable->type == variable_string)
			symbol_variable_assign_string(dst->ptr.variable, src->ptr.variable->value.value_string);
	}

	dst->right = NULL;
	dst->left = NULL;
}

/**
 * Copy a binary search tree
 * 
 * @return   poniter to a newly created tree, or NULL if given tree is NULL
 */
bst_node * bst_copy(bst_node *root)
{
	if (root == NULL)
		return NULL;

	bst_node *target = (bst_node *) malloc(sizeof(bst_node));
	alloc_check((void *) target, "target node");
	bst_node *res = target;
	node_stack stack_target;
	stack_init(&stack_target);
	node_stack stack_orig;
	stack_init(&stack_orig);

	while (1)
	{
		bst_node_variable_copy(target, root);

		if (root->right != NULL)
		{
			stack_push(&stack_orig, root->right);
			target->right = (bst_node *) malloc(sizeof(bst_node));
			alloc_check((void *) target->right, "target node");
			stack_push(&stack_target, target->right);
		}

		if (root->left != NULL)
		{
			target->left = (bst_node *) malloc(sizeof(bst_node));
			alloc_check((void *) target->left, "target node");
			root = root->left;
			target = target->left;
		}
		else
		{
			if (stack_orig.top >= 0)
			{
				root = stack_pop(&stack_orig);
				target = stack_pop(&stack_target);
			}
			else
			{
				stack_dest(&stack_orig);
				stack_dest(&stack_target);
				return res;
			}
		}
	}
}

/**
 * Debuging function for printing node to stdout
 */
void bst_node_print(bst_node *node)
{
	switch (node->type)
	{
		case type_variable:
			printf("VAR   '%s'\t", node->key);
			switch (node->ptr.variable->type)
			{
				case variable_integer:
					printf(" integer ");
					break;

				case variable_double:
					printf(" double  ");
					break;

				case variable_string:
					printf(" string  ");
					break;

				default:
					fprintf(stderr, "Error - unknown type of variable!\n");
					exit(ERR_INTERNAL);
			}
			if (node->ptr.variable->init == 0)
				printf("UNINITIALIZED\n");
			else
				printf("INITIALIZED\n");
			break;

		case type_function:
			printf("FUNC  '%s'\t", node->key);
			switch (node->ptr.function->return_type)
			{
				case variable_integer:
					printf(" integer\n");
					break;

				case variable_double:
					printf(" double\n");
					break;

				case variable_string:
					printf(" string\n");
					break;

				case variable_void:
					printf(" void\n");
					break;

				default:
					fprintf(stderr, "Error - unknown type of function!\n");
					exit(ERR_INTERNAL);
			}

			bst_inorder(node->ptr.function->local_table, "\t\t-> ");
			printf("\n");
			break;

		case type_class:
			printf("CLASS '%s'\n", node->key);
			bst_inorder(node->ptr.global,"\t-> ");
			printf("\n");
			break;

		default:
			fprintf(stderr, "Error - unknown type of node!\n");
			exit(ERR_INTERNAL);
	}
}

/**
 * Debuging function for printing tree in order
 */
void bst_inorder(bst_node *root, char * pre_str)
{
	if (root == NULL)
		return;

	bst_inorder(root->left, pre_str);

	printf("%s",pre_str);
	bst_node_print(root);

	bst_inorder(root->right, pre_str);
}

/**
 * Free the node and all of its alocated content
 */
void bst_node_free(bst_node *node)
{
	free(node->key);

	if (node->type == type_variable)
	{
		free(node->ptr.variable->name);
		if (node->ptr.variable->type == variable_string)
			free(node->ptr.variable->value.value_string);

		free(node->ptr.variable);
	}
	else if (node->type == type_function)
	{
		free(node->ptr.function->name);
		function_args_free(node->ptr.function->args, node->ptr.function->args_count);
		dispose_list (node->ptr.function->func_ins_list);
		bst_dispose(&(node->ptr.function->local_table));

		free(node->ptr.function);
	}
	else
	{
		bst_dispose(&(node->ptr.global));
	}

	free(node);
}

/**
 * Call void function over each node of a tree
 */
void bst_foreach(bst_node **root, void (*func)())
{
	if (*root == NULL)
		return;

	node_stack stack;
	stack_init(&stack);
	bst_node *node;

	while ((*root != NULL) || (stack.top >= 0))
	{
		if (*root == NULL)
		{
			*root = stack_pop(&stack);
		}
		else
		{
			if ((*root)->right != NULL)
				stack_push(&stack, (*root)->right);

			node = *root;
			*root = (*root)->left;
			func(node);
		}
	}

	stack_dest(&stack);
}

/**
 * Dispose the given tree and return it to a state as after initialization
 */
void bst_dispose(bst_node **root)
{
	bst_foreach(root, bst_node_free);
}

/* ----------- Search and sort algorithms ----------- */

/**
 * Determine KMP graph for a substring.
 *
 * @return   pointer on the first element of graph.
 */
int * kmp_graph(char *s)
{
	int len = strlen(s);
	int i, r;
	int *f = malloc(len * sizeof(int));
	alloc_check((void *) f, "kmp graph");

	f[0] = -1;

	for (i = 1; i < len; i++)
	{
		r = f[i-1];
		if ((r >= 0) && (s[r] != s[i-1]))
			r = f[r];

		f[i] = r+1;
	}

	return f;
}

/**
 * Find match of substring in string using KMP algorithm.
 *
 * @return   position of the substring in string, or -1 if not found
 */
int kmp_match(char *b, char *s, int *f)
{
	int blen = strlen(b);
	int slen = strlen(s);
	int bi = 0;
	int si = 0;

	while ((bi < blen) && (si < slen))
	{
		if ((si == -1) || (b[bi] == s[si]))
		{
			bi += 1;
			si += 1;
		}
		else
		{
			si = f[si];
		}
	}

	if (si == slen)
		return bi-slen;

	else
		return -1;
}

/**
 * Partition given string for quick sort.
 */
void qs_partition(char *s, int l, int r, int *i, int *j)
{
	char p;
	char m = s[(l+r) / 2];
	*i = l;
	*j = r;

	while (*j >= *i)
	{
		while (s[*i] < m)
			*i += 1;

		while (s[*j] > m)
			*j -= 1;

		if (*i <= *j)
		{
			p = s[*i];
			s[*i] = s[*j];
			s[*j] = p;
			*i += 1;
			*j -= 1;
		}
	}
}

/**
 * Sort string by ordinary values using quick sort.
 */
void qs_sort(char *s, int l, int r)
{
	int i, j;
	qs_partition(s, l, r, &i, &j);

	if (l < j)
		qs_sort(s, l, j);

	if (r > i)
		qs_sort(s, i, r);
}
