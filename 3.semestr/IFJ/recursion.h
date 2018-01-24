/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef RECURSION_H
#define RECURSION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ial.h"
#include "symbol.h"
#include "stack.h"
#include "error.h"

/* ----- Function prototypes ----- */

int symbol_apply(symbol_variable *dst, symbol_variable *src);
/* Auxiliary function - copies value of a given symbol to another symbol.
 * Returns 0 if successful, otherwise ERR_INTERNAL. */

void bst_node_clean(bst_node *node);
/* Auxiliary function - cleans (uninitializes) a given variable node. */

void assign_args(symbol_function *func);
/* Copies argument values to the local table. Give it pointer on function symbol. */

void make_mask(bst_node *local);
/* Makes a mask of local table, pushes it to mask stack and cleans the original local table.
 * Give it pointer to the local table of function. */

void apply_mask(symbol_function *);
/* Applies mask from top of stack to local table. Give it pointer on it. */

void dispose_mask_stack();
/* Disposes mask stack in case of error */

#endif