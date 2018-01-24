/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef stack_h
#define stack_h

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

// define type Stack_item - one item from linked list used like stack
typedef struct S_item {
    void * data_ptr;
    struct S_item * next;
} Stack_item;

//defice type Stack with pointer to first element
typedef struct {
    Stack_item * first;
} Stack;

extern Stack Garbage;

// call before start using stack
// setup default value of first item
void Stack_init(Stack *S);

//free the first item
//edit by Vlado, because it is good stack implementation
void Stack_pop(Stack *S);

//Function for return the top item
//add by Vlado, because this function missing :)
void * Stack_top(Stack *S);

// create new element of linked list
// S->data_ptr=ptr
// get new element on top of stack
// All stored data must be created by malloc !!
int Stack_push(Stack *S,void *ptr);

// call free on all elements and their data =>>> Cant store two same pointers !!!
void Stack_dispose(Stack *S);

void Stack_not_free_pop(Stack *S);

#endif
