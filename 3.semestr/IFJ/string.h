/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef string_h
#define string_h

#include <stdlib.h>
#include <stdio.h>
#include "error.h"

#define STR_SIZE  8

/*
 * data main item in struct
 * size - size of actual allocated space
 * pos - actual position of last char in string
 * rall_count - count of reallocation
 */
typedef struct str
{
    char * data;
    unsigned size;
    unsigned pos;
    unsigned rall_count;
} t_String;


int StrInitString(t_String * S);
/*
 * create and malloc String element
 * is necessary use ths function before start using string
 */
int StrPutCharToString(char input, t_String *  S);
/*
 * get input char into string S
 */
int StrRemove(t_String *S);
/*
 * clean all data from struct T_string
 */
#endif
