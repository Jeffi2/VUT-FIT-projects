/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef lltable_h
#define lltable_h

//Definition of LL table

#include "scaner.h"
#include "error.h"
#include "sym_table.h"
#include "ial.h"
#include "expressions.h"
#include "constants.h"
#include "symbol.h"
#include "call_function.h"
#include "generate.h"

extern int First_look;





//non terminal <prog>
int prog();

//non terminal <inclass>
int inclass();
//non terminal <decl>
int decl();
//non terminal <varfunc>
int varfunc(tType_id item);
//non terminal <func>
int func(tType_id func);

//non terminal <parlist>
int parlist();
//non terminal <paritem>
int paritem();
//non terminal <infunc>
int infunc();
//non terminal <locvar>
int locvar(tType_id local_var);

//non terminal <stat>
int stat();
int is_else(ins_item_ptr if_else, ins_item_ptr if_end);

//non terminal <assfunc>
int assfunc(tType_id item);

//non terminal <compstat>
int compstat();
//non terminal <incompstat>
int incompstat();

//non terminal <type>
int type();

//non terminal <assign>
int assign(symbol_variable * var);

//non terminal <expr>
int expr_non();
int expr(symbol_variable * help_var);

//print err
void print_error(const char * str);





#endif /* lltable_h */
