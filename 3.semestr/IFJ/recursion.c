/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "recursion.h"

extern Stack *mask_stack;

/**
 * Copy value of a given symbol to another symbol.
 * 
 * @return 0 if successful, otherwise ERR_INTERNAL
 */
int symbol_apply(symbol_variable *dst, symbol_variable *src)
{
	switch (src->type)
	{
		case variable_integer:	
			if(symbol_variable_assign_int(dst, src->value.value_integer) == TYPE_ERR)
				return ERR_INTERNAL; //because this should never happen
			break;

		case variable_double:
			if(symbol_variable_assign_double(dst, src->value.value_double) == TYPE_ERR)
				return ERR_INTERNAL;
			break;
			
		case variable_string:
			if(symbol_variable_assign_string(dst, src->value.value_string) == TYPE_ERR)
				return ERR_INTERNAL;
			break;

		default:
			return ERR_INTERNAL;
	}

	return 0;
}

/**
 * Clean (uninitialize) a given variable node.
 */
void bst_node_clean(bst_node *node)
{
	node->ptr.variable->init = 0;

	switch (node->ptr.variable->type)
	{
		case variable_integer:
			node->ptr.variable->value.value_integer = -1;
			break;

		case variable_double:
			node->ptr.variable->value.value_double = -1;
			break;
			
		case variable_string:
			free(node->ptr.variable->value.value_string);
			node->ptr.variable->value.value_string = NULL;
			break;

		default: //this should never ever happen
			fprintf(stderr, "Error - unknown type of variable!\n");
			exit(ERR_INTERNAL); 
	}
}

/**
 * Copy argument values to the local table.
 */
void assign_args(symbol_function *func)
{
	unsigned int i;
	bst_node *node;

	for (i = 0; i < func->args_count; i++)
	{
		node = bst_node_find(func->local_table, func->args[i].name);
		symbol_apply(node->ptr.variable, &(func->args[i])); //should check for err?
	}
}

/**
 * Make mask of local table, push it on mask stack and clean the original table.
 */
void make_mask(bst_node *local)
{
	bst_node *mask = bst_copy(local);
	Stack_push(mask_stack, (void *) mask);
	bst_foreach(&local, bst_node_clean);
}

/**
 * Apply mask from top of stack to local table.
 */
void apply_mask(symbol_function *funn)
{
    bst_node *local = funn->local_table;
	bst_node *mask = (bst_node *) Stack_top(mask_stack);
	if (mask == NULL)
		return;

	node_stack stack_local;
	stack_init(&stack_local);
	node_stack stack_mask;
	stack_init(&stack_mask);

	while (1)
	{
		symbol_apply(local->ptr.variable, mask->ptr.variable); //should check for err?

		if (mask->right != NULL)
		{
			stack_push(&stack_mask, mask->right);
			stack_push(&stack_local, local->right);
		}

		if (mask->left != NULL)
		{
			mask = mask->left;
			local = local->left;
		}
		else
		{
			if (stack_mask.top >= 0)
			{
				mask = stack_pop(&stack_mask);
				local = stack_pop(&stack_local);
			}
			else
			{
				stack_dest(&stack_mask);
				stack_dest(&stack_local);
				mask = (bst_node *) Stack_top(mask_stack);
				bst_dispose(&mask);

				Stack_item *del;
				del = mask_stack->first;
				mask_stack->first = mask_stack->first->next;
				free(del);
				return;
			}
		}
	}
}

/**
 * Dispose mask stack in case of error.
 */
void dispose_mask_stack()
{
	bst_node *mask;
	Stack_item *del;

	while (mask_stack->first != NULL)
	{
		mask = (bst_node *) Stack_top(mask_stack);
		bst_dispose(&mask);

		del = mask_stack->first;
		mask_stack->first = mask_stack->first->next;
		free(del);
	}
}