/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef IAL_H
#define IAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"
#include "error.h"
#include "generate.h"

typedef enum {
	type_variable,
	type_function,
	type_class
} item_type;

/* ----- Structure definitions ----- */

struct Tsymbol_variable;
struct Tsymbol_function;

typedef struct bst_node_s {
	char *key;
	item_type type;
	union {
		struct Tsymbol_variable *variable;
		struct Tsymbol_function *function;
		struct bst_node_s *global;
	} ptr;
	struct bst_node_s *left;
	struct bst_node_s *right;
} bst_node;

typedef struct {
	bst_node **arr;
	int top;
	int max;
} node_stack;

/* ----- Function prototypes ----- */

void alloc_check(void *ptr, char *msg);
void stack_init(node_stack *stack);
void stack_push(node_stack *stack, bst_node *node);
bst_node * stack_pop(node_stack *stack);
void stack_dest(node_stack *stack);
/* Auxiliary functions */

void bst_init(bst_node **root);
/* Initializes the tree. Points given pointer to null. */

void bst_class_init(bst_node *node, char *name);
/* Auxiliary function for setting initial values to class node */

int bst_insert_class(bst_node **root, char *key);
/* Inserts class node to the tree and sets initial values.
 * Give it root of the tree and name of the class.
 * Returns 0 if succesful, SEM_ERR if the key already exists. */

int bst_insert_variable(bst_node **root, char *key, int type); // int type = variable_type type
/* Inserts variable node to the tree and sets initial values.
 * Give it root of the tree, a name of the variable and its type -> see symbol.h -> variable_type
 * Returns 0 if succesful, SEM_ERR if the key already exists. */

int bst_insert_function(bst_node **root, char *key, int type);
/* Inserts function node to the tree and sets initial values.
 * Give it root of the tree, a name of the function, its return type and offset.
 * Returns 0 if succesful, SEM_ERR if the key already exists. */

int bst_node_add_arg(bst_node *node, char *name, int type);
/* Adds argument to the list of arguments of function, and to it's local table.
 * Give it a pointer to node of function, name and type of argument.
 * Returns 0 if succesful, SEM_ERR if arg with given name already exists, ERR_INTERNAL if given node is not function */

bst_node * bst_node_find(bst_node *root, char *key);
/* Finds the node with given key in given tree and returns pointer on it.
 * If node with such key doesn't exist, it returns NULL. */

int bst_node_is_defined(bst_node *root, char *key);
/* Checks if variable is defined (initialized). Give it root of tree and key of node.
 * Returns 0 if defined, 1 if undefined, ERR_INTERNAL if node with given key doesn't exist or is function. */

void bst_node_define(bst_node *root, char *key);
/* Make variable defined (initialized). Give it root of tree and key of node. */

void bst_node_variable_copy(bst_node *dst, bst_node *src);
/* Auxiliary function for copying nodes. */

bst_node * bst_copy(bst_node *root);
/* Makes a copy of given tree and returns pointer on the root of the copy. Use for local table. */

void bst_node_print(bst_node *node);
/* Debug auxiliary function for printing a node to stdout. Give it pointer on node. */

void bst_inorder(bst_node *root, char * pre_str);
/* Debug function for printing out all tree nodes in order. Give it root of tree. */

void bst_node_free(bst_node *node);
/* Auxiliary function for freeing a node. */

void bst_foreach(bst_node **root, void (*func)());
/* Calls a void function on each node of given tree. */

void bst_dispose(bst_node **root);
/* Disposes a tree and retrurns it to initial state. !HAS SOME VALGRIND ERRORS! */

int * kmp_graph(char *s);
/* Returns pointer on the first element of a graph of given string for KMP algorithm. */

int kmp_match(char *b, char *s, int *f);
/* Returns first position of substring in string using KMP algorithm, or -1 if not found.
 * Give it base string, substring and fail graph of the substring. */

void qs_partition(char *s, int l, int r, int *i, int *j);
/* Partitions given string for quick sort. Give it string, left and right boundaries,
 * and pointers on partitioning indexes. */

void qs_sort(char *s, int l, int r);
/* Sorts string by ordinary values using quick sort. Give it string to be sorted
 * and left and right boundaries of the string. */

#endif