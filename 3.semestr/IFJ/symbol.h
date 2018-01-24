/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ial.h"
#include "error.h"
#include "generate.h"

struct instruction_list;

typedef enum {
	variable_integer,
	variable_double,
	variable_string,
	variable_void,
    variable_bool
} variable_type;

/* ----- Structure definition ----- */

struct bst_node_s;

/*typedef struct {
	variable_type type;
	char *name;
} function_arg;*/

typedef struct Tsymbol_variable {
	int init;
	char *name;
	variable_type type;
	union {
		double value_double;
		int value_integer;
		char *value_string;
        bool value_bool;
	} value;
} symbol_variable;

typedef struct Tsymbol_function {
	int defined;
	char *name;
	variable_type return_type;
	unsigned int args_count;
	unsigned int act_arg;
	symbol_variable *args;
    struct instruction_list *func_ins_list;
	struct bst_node_s *local_table;
} symbol_function;

/* ----- Function prototypes ----- */

void symbol_variable_init(struct bst_node_s *var, char *name, variable_type type);
/* Auxiliary function for initializing a variable. */

int symbol_variable_assign_int(symbol_variable *var, int value);
int symbol_variable_assign_double(symbol_variable *var, double value);
int symbol_variable_assign_string(symbol_variable *var, char *value);
int symbol_variable_concat_string(symbol_variable *var, char *value);
/* Assigning functions. After you find a node, you can assign a value to it using these functions.
 * Choose a function according to the type and give it a pointer to the symbol and a value.
 * If the type doesn't match, returns TYPE_ERR, otherwise 0 */

int symbol_variable_value_int(symbol_variable *var);
double symbol_variable_value_double(symbol_variable *var);
char * symbol_variable_value_string(symbol_variable *var);
/* These functions get a value of a symbol. Choode a function according to type
 * and give it a poointer to the symbol. Returns the value, but if types doesn't match
 * returns NONINI_ERR or NULL in case of string. */

void symbol_function_init(struct bst_node_s *var, char *name, variable_type type);
/* Auxiliary function for initializing a variable. */

void symbol_function_arg_add(symbol_function *var, char *name, variable_type type);
/* Adds an argument to function. Give it pointer on the symbol, name and type of the argument. */

void function_args_free(symbol_variable *args, unsigned int args_count);
/* Auxiliary function for freeing arguments. */

#endif
