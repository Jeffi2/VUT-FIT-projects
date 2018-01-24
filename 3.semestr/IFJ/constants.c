/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "constants.h"
#include "symbol.h"

Stack const_table;

symbol_variable * make_const(t_token *const_token)
{
    symbol_variable * item;

    switch(const_token->state)
    {
        case S_INT:
            item = malloc(sizeof(symbol_variable));
            if(item == NULL)
            {
                fprintf(stderr, "error in make const in malloc\n");
                exit(ERR_INTERNAL);
            }

            item->init = 1;
            item->type = variable_integer;
            item->value.value_integer = const_token->int_data;
            item->name = NULL;

            //push to garbage
            Stack_push(&Garbage, (void * ) item);

            return item;

        case S_DOUBLE:
            item = malloc(sizeof(symbol_variable));
            if(item == NULL)
            {
                fprintf(stderr, "error in make const in malloc\n");
                exit(ERR_INTERNAL);
            }

            item->init = 1;
            item->type = variable_double;
            item->value.value_double = const_token->double_data;
            item->name = NULL;

            //push to garbage
            Stack_push(&Garbage, (void * ) item);

            return item;

        case S_STR:
            item = malloc(sizeof(symbol_variable));
            if(item == NULL)
            {
                fprintf(stderr, "error in make const in malloc\n");
                exit(ERR_INTERNAL);
            }

            item->type = variable_string;
            item->value.value_string = NULL;

            if(symbol_variable_assign_string(item, const_token->string_data.data) != OK)
            {
                fprintf(stderr,"error in string in make constant");
                exit(ERR_INTERNAL);
            }

            //push to const_table -- because non normal dispose -> string contain
            Stack_push(&const_table, item);

            return item;

        default:
            fprintf(stderr, "error in make constant with this token : %d\n", const_token->state);
            exit(ERR_INTERNAL);
    }
}


symbol_variable * make_help_var()
{
    symbol_variable *item;

    item = malloc(sizeof(symbol_variable));
    if(item == NULL)
    {
        fprintf(stderr, "error in make const in malloc\n");
        exit(ERR_INTERNAL);
    }

    item->init = 0;
    item->type = variable_void;
    item->name = NULL; //without any name --> it is useless
    item->value.value_string = NULL;

    Stack_push(&const_table, item);

    return item;
}




void dispose_const_table()
{
    symbol_variable * item = (symbol_variable *) Stack_top(&const_table);
    while(item != NULL)
    {
        if(item->type == variable_string && item->value.value_string != NULL)
            free(item->value.value_string);

        Stack_pop(&const_table);
        item = (symbol_variable *) Stack_top(&const_table);
    }
}
