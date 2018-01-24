/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

//project includes
#include "stack.h"
#include "symbol.h"
#include "scaner.h"

extern Stack const_table;

//function, which make constant from token
//return pointer to symbol variable
//note: char * name is NULL
symbol_variable * make_const(t_token *const_token);


//function, which make help variable
//return pointer to help variable
//note: char * name is NULL and
//type is variable_void
//value is NULL
//init is also 0
symbol_variable * make_help_var();

//function to dispose const table
//first clear char * string;
//after that clean item
void dispose_const_table();

#endif //CONSTANTS_H
