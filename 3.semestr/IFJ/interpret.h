/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef interpret_h
#define interpret_h

#include "string.h"
#include "generate.h"
#include "stack.h"
#include "error.h"
#include "symbol.h"
#include "recursion.h"
#include "builtin.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

extern symbol_variable *RETURN;

int interpret(ins_list* list);
int math(int c, symbol_variable *adr1, symbol_variable *adr2, symbol_variable *adr3);
int compare(char c, symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3);
int negate(symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3);
int builtinfc(int i, symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3);
int go_to(ins_list* list, symbol_variable* adr1, void *adr2, ins_item_ptr ptr);
int assign_value(symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3, int cnt);
int ass_return(symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3);

void dispose_instr_next();

#endif /* interpret_h */
