/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "string.h"
#include "stack.h"
extern Stack Garbage;
extern FILE *file;

int StrInitString(t_String * S)
{
    S->data=malloc(sizeof(char)*STR_SIZE);
    if (S->data == NULL)
    {
        fprintf(stderr, "Error at allocation!\n");
        Stack_dispose(&Garbage);
        fclose(file);
        exit(ERR_INTERNAL);
    }

    S->size = STR_SIZE;
    S->pos = 0;
    S->rall_count = 1;

    return 0;
}

int StrPutCharToString(char input, t_String *  S)
{
    if (S->size-2 == S->pos)
    {
        S->rall_count++;
        S->data = realloc(S->data, S->rall_count * sizeof(char) * STR_SIZE);
        if (S->data == NULL)      //Taky se nemusí povést
        {
            fprintf(stderr, "Error at allocation!\n");
            Stack_dispose(&Garbage);
            fclose(file);
            exit(ERR_INTERNAL);
        }
        S->size=STR_SIZE * S->rall_count;

    }
    S->data[S->pos] = input;
    S->pos ++ ;
    
    return 0;
    }

int StrRemove(t_String *S) //Shoud not use with strings in garbage
{

    if (S->data != NULL)
        free(S->data);
    else
        return 1;
    
    S->rall_count = 0;
    S->pos = 0;
    S->size = 0;

    return 0;
}