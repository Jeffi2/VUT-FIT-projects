/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef CALL_FUNCTION_H
#define CALL_FUNCTION_H

#include "symbol.h"
#include "sym_table.h"
#include "scaner.h"
#include "lltable.h"
#include "generate.h"
#include "interpret.h"

#define num_readInt -1
#define num_readDouble -2
#define num_readString -3
#define num_print -4
#define num_length -5
#define num_substr -6
#define num_compare -7
#define num_find -8
#define num_sort -9


int func_call(symbol_function * fun, symbol_variable * var);

int params(symbol_function * fun);
int more_param(symbol_function * fun);

int value(symbol_function * fun);





int cat();


#endif //CALL_FUNCTION_H
