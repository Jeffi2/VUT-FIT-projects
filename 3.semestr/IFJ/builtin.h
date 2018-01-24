/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef BUILT_IN
#define BUILT_IN

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "error.h"
#include "ial.h"

int readInt();
/* Reads integer from standard input. Must be non-empty integer. */

double readDouble();
/* Reads double from standard input. For definition of double look to doc. */

char * readString();
/* Reads string from standard input until \n or EOF is encountered,
 * which won't be part of string. Empty string is possible. */

void print(symbol_variable *ptr);
/* Prints the contents of variable to stdout. Give it a pointer on variable symbol */

int length(char *s);
/* Returns length of given string. If NULL is given, returns -1. */

char * substr(char *s, int i, int n);
/* Returns a substring of given string. Substring starts at position given by i and is n chars long.
 * If part after length of s is cut off and if i is negative, the result is NULL. */

int compare_string(char *s1, char *s2);
/* Compares two strings. Returns 0 if they are equal, 1 if s1 is bigger, -1 if s2 is bigger. */

int find(char *s, char *search);
/* Finds substring in string. Give it base string and substring.
 * Returns position of first occurence of substring in string, or -1 if no match is found. */

char * sort(char *s);
/* Sorts characters of string. Makes a copy of given string and sorts the characters, then returns it. */

#endif
