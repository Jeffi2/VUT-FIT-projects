/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/
#include "stack.h"

extern FILE *file;

//global variable
Stack Garbage;

void Stack_init(Stack *S)
{
    S->first = NULL;
    return;
}

void Stack_pop(Stack *S)
{
    if (S->first == NULL)
        return;

    Stack_item *tmp;
    tmp = S->first;
    S->first = S->first->next;
    free(tmp->data_ptr);
    free(tmp);
}

void Stack_not_free_pop(Stack *S)
{
    if (S->first == NULL)
        return;
    
    Stack_item *tmp;
    tmp = S->first;
    S->first = S->first->next;
   // free(tmp->data_ptr);
    free(tmp);
}


void * Stack_top(Stack *S)
{
    void * tmp;
    if (S->first == NULL)
    {
        return NULL;
    }

    tmp = S->first->data_ptr;
    return tmp;
}

int Stack_push(Stack *S, void * ptr)
{
    Stack_item * tmp;
    tmp = malloc(sizeof(struct S_item));
    if (tmp == NULL)
    {
        Stack_dispose(&Garbage);
        fclose(file);
        exit(ERR_INTERNAL);
    }
    tmp->data_ptr=ptr;
    tmp->next=S->first;
    S->first=tmp;
    return 1;
}

void Stack_dispose(Stack *S)
{
    if (S->first == NULL)
        return;
    
    Stack_item *tmp;
    while(S->first != NULL)
    {
        tmp = S->first;
        S->first = S->first->next;
        free(tmp->data_ptr);
        free(tmp);

    }
}
