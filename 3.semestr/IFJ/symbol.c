/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "symbol.h"

/**
 * Set initial values to node for variable
 */
void symbol_variable_init(bst_node *node, char *name, variable_type type)
{
	node->key = (char *) malloc(strlen(name) + 1);
	alloc_check((void *) node->key, "variable key");
	strcpy(node->key, name);
	node->type = type_variable;

	node->ptr.variable = (symbol_variable *) malloc(sizeof(symbol_variable));
	alloc_check((void *) node->ptr.variable, "variable symbol");

	node->ptr.variable->name = (char *) malloc(strlen(name) + 1);
	alloc_check((void *) node->ptr.variable->name, "variable name");
	strcpy(node->ptr.variable->name, name);
	node->ptr.variable->init = 0;
	node->ptr.variable->type = type;

	if (type == variable_string)
		node->ptr.variable->value.value_string = NULL;
}

/**
 * Set variable value to an integer
 * 
 * @return   0 if succesful, TYPE_ERR if incorrect variable type
 */
int symbol_variable_assign_int(symbol_variable *var, int value)
{
	if (var->type != variable_integer)
		return TYPE_ERR;

	var->value.value_integer = value;
	var->init = 1;
	return 0;
}

/**
 * Set variable value to a double
 * 
 * @return   0 if succesful, TYPE_ERR if incorrect variable type
 */
int symbol_variable_assign_double(symbol_variable *var, double value)
{
	if (var->type != variable_double)
		return TYPE_ERR;

	var->value.value_double = value;
	var->init = 1;
	return 0;
}

/**
 * Set variable value to a string
 * 
 * @return   0 if succesful, TYPE_ERR if incorrect variable type
 */
int symbol_variable_assign_string(symbol_variable *var, char *value)
{
	if (var->type != variable_string)
		return TYPE_ERR;

	if (value == NULL)
	{
		//because apply of mask
		if (var->value.value_string != NULL)
			free(var->value.value_string);

		var->value.value_string = NULL;
		return OK;
	}

	if (var->value.value_string == NULL)
	{
		var->value.value_string = (char *) malloc(strlen(value) + 1);
		alloc_check((void *) var->value.value_string, "string");
	}
	else if (strlen(value) > strlen(var->value.value_string))
	{
		var->value.value_string = (char *) realloc(var->value.value_string, strlen(value) + 1);
		alloc_check((void *) var->value.value_string, "string");
	}

	strcpy(var->value.value_string, value);
	var->init = 1;
	return 0;
}

/**
 * Add string to the end of variable
 * 
 * @return   0 if succesful, TYPE_ERR if incorrect variable type
 */
int symbol_variable_concat_string(symbol_variable *var, char *value)
{
	if (var->type != variable_string)
		return TYPE_ERR;

	if (var->value.value_string == NULL)
	{
		var->value.value_string = (char *) malloc(strlen(value) + 1);
		alloc_check((void *) var->value.value_string, "string");
	}
	else
	{
		var->value.value_string = (char *) realloc(var->value.value_string, strlen(var->value.value_string) + strlen(value) + 1);
		alloc_check((void *) var->value.value_string, "string");
	}

	strcat(var->value.value_string, value);
	var->init = 1;
	return 0;
}

/**
 * Get the integer value stored in given variable
 * 
 * @return   value of given variable, NONINI_ERR if not initialized
 */
int symbol_variable_value_int(symbol_variable *var)
{
	if (var->init == 0)
		return NONINI_ERR;

	return var->value.value_integer;
}

/**
 * Get the double value stored in given variable
 * 
 * @return   value of given variable, NONINI_ERR if not initialized
 */
double symbol_variable_value_double(symbol_variable *var)
{
	if (var->init == 0)
		return NONINI_ERR;

	return var->value.value_double;
}

/**
 * Get the string value stored in given variable
 * 
 * @return   value of given variable, NULL if not initialized
 */
char * symbol_variable_value_string(symbol_variable *var)
{
	if (var->init == 0)
		return NULL;

	return var->value.value_string;
}

/**
 * Set initial values to node for function
 */
void symbol_function_init(bst_node *node, char *name, variable_type type)
{
	node->key = (char *) malloc(strlen(name) + 1);
	alloc_check((void *) node->key, "function key");
	strcpy(node->key, name);
	node->type = type_function;

	node->ptr.function = (symbol_function *) malloc(sizeof(symbol_function));
	alloc_check((void *) node->ptr.function, "function symbol");

	node->ptr.function->name = (char *) malloc(strlen(name) + 1);
	alloc_check((void *) node->ptr.function->name, "function name");
	strcpy(node->ptr.function->name, name);
	node->ptr.function->defined = 1;
	node->ptr.function->return_type = type;
	node->ptr.function->args_count = 0;
	node->ptr.function->act_arg = 0;
	node->ptr.function->args = NULL;
	node->ptr.function->func_ins_list = create_fnc_list();
	node->ptr.function->local_table = NULL;
}

/**
 * Add one more argument to function symbol
 */
void symbol_function_arg_add(symbol_function *func, char *name, variable_type type)
{
	func->args_count += 1;

	if (func->args_count == 1)
	{
		func->args = (symbol_variable *) malloc(sizeof(symbol_variable));
		alloc_check((void *) func->args, "argument");
	}
	else
	{
		func->args = (symbol_variable *) realloc(func->args, sizeof(symbol_variable) * func->args_count);
		alloc_check((void *) func->args, "argument");
	}

	func->args[func->args_count - 1].name = (char *) malloc(strlen(name) + 1);
	alloc_check((void *) func->args[func->args_count - 1].name, "argument name");
	strcpy(func->args[func->args_count - 1].name, name);
	func->args[func->args_count - 1].type = type;

	if (type == variable_string)
		func->args[func->args_count - 1].value.value_string = NULL;
}

/**
 * Free allocated memory for arguments structure
 */
void function_args_free(symbol_variable *args, unsigned int args_count)
{
	unsigned int i;
	for (i = 0; i < args_count; i++)
	{
		free(args[i].name);

		if (args[i].type == variable_string)
			free(args[i].value.value_string);
	}

	free(args);
}
