/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef PARSER_H
#define PARSER_H

//include standard libraries
#include <stdio.h>
#include <stdlib.h>

//include local  libraries
#include "scaner.h"
#include "error.h"
#include "ial.h"
#include "lltable.h"


extern Stack Garbage;
extern t_token * token;



//parse is function, which check syntax and gen. instruction of  the input file
int parse(tSymbolTable * ST);

//function, which add built in function and class to symbol table
int add_built_in();

//function to control if input file has Main.run correct
//return true or false
int has_main_run();

#endif //PARSER_H
