/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "call_function.h"

#define FALSE 0
#define TRUE 1

// -> ( <params> )
int func_call(symbol_function * fun, symbol_variable * var)
{
    int result;
    //help var is for builtin function
    symbol_variable * help_var;
    symbol_variable * term;
    symbol_variable * arg1;
    symbol_variable * arg2;
    symbol_variable * var1;

    fun->act_arg = 0;

    if( var == NULL)
        help_var = make_help_var();
    else
    {
        help_var = var;

        if (fun->return_type != var->type) return TYPE_ERR;
    }


    //expected (
    if (token->state != S_LTHE) return SYNTAX_ERR;
    //next token
    if (get_next_token(token) == LEX_ERR) return LEX_ERR;

    switch (fun->defined)
    {
        case num_readInt:

            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                //it is unexpected, but TYPE_ERR
                case S_ID:
                case S_ID_FULL:
                    //check if exist? , becuse sem_err has more preference
                    var1 = find_var(token->string_data.data);
                    if(var1 == NULL) return SEM_ERR;

                case S_INT:
                case S_DOUBLE:
                case S_STR:
                    return TYPE_ERR;

                //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, NULL, NULL, help_var, I_READ_INT);
            return OK;

        case num_readDouble:
            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                    //it is unexpected, but TYPE_ERR
                case S_ID:
                case S_ID_FULL:
                    var1 = find_var(token->string_data.data);
                    if(var1 == NULL) return SEM_ERR;

                case S_INT:
                case S_DOUBLE:
                case S_STR:
                    return TYPE_ERR;

                    //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, NULL, NULL, help_var, I_READ_D);
            return OK;

        case num_readString:
            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                    //it is unexpected, but TYPE_ERR
                case S_ID:
                case S_ID_FULL:
                    var1 = find_var(token->string_data.data);
                    if(var1 == NULL) return SEM_ERR;

                case S_INT:
                case S_DOUBLE:
                case S_STR:
                    return TYPE_ERR;

                    //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, NULL, NULL, help_var, I_READ_STR);
            return OK;

        case num_print:

            switch (token->state)
            {
                case S_ID:
                case S_ID_FULL:
                    term = find_var(token->string_data.data);
                    if(term == NULL) return SEM_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                case S_STR:
                    term = make_const(token);
                    break;

                default:
                    return SYNTAX_ERR;
            }

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, term, NULL, NULL, I_PRINT);

            result = cat();
            if (result != OK) return result;

            //expected )
            if (token->state != S_RTHE) return SYNTAX_ERR;
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        case num_length:
            switch (token->state)
            {
                case S_STR:
                    arg1 = make_const(token);
                    break;

                case S_ID:
                case S_ID_FULL:
                    arg1 = find_var(token->string_data.data);
                    if(arg1 == NULL) return SEM_ERR;

                    if(arg1->type != variable_string) return TYPE_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                    return TYPE_ERR;

                default:
                    return SYNTAX_ERR;
            }

            // STRING s is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                //when is ,
                case S_COMMA:
                    if (get_next_token(token) == LEX_ERR) return LEX_ERR;

                    switch (token->state)
                    {
                        case S_ID:
                        case S_ID_FULL:
                            var1 = find_var(token->string_data.data);
                            if(var1 == NULL) return SEM_ERR;

                        case S_INT:
                        case S_DOUBLE:
                        case S_STR:
                            return TYPE_ERR;

                        default:
                            return SYNTAX_ERR;
                    }

                //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            // ) is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, arg1, NULL, help_var, I_LENGTH);

            return  OK;

        case num_substr:
            //clear arguments
            fun->act_arg = 0;
            //expected String s
            result = value(fun);
            if (result != OK) return result;

            //expected ,
            if (token->state != S_COMMA) return SYNTAX_ERR;
            //next  token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //expected int i
            result = value(fun);
            if (result != OK) return result;

            //expected ,
            if (token->state != S_COMMA) return SYNTAX_ERR;
            //next  token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //expected int n
            result = value(fun);
            if (result != OK) return result;

            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                    //when is ,
                case S_COMMA:
                    if (get_next_token(token) == LEX_ERR) return LEX_ERR;

                    switch (token->state)
                    {
                        case S_ID:
                        case S_ID_FULL:
                            var1 = find_var(token->string_data.data);
                            if(var1 == NULL) return SEM_ERR;

                        case S_INT:
                        case S_DOUBLE:
                        case S_STR:
                            return TYPE_ERR;

                        default:
                            return SYNTAX_ERR;
                    }

                    //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            // ) is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //check number of args
            if(fun->act_arg != fun->args_count)
            {
                fprintf(stderr, "Few or more arguments of call function: %s \n", fun->name);
                return TYPE_ERR;
            }

            generate_instruction(INS_LIST, fun->args, NULL , help_var, I_SUBSTR);
            return  OK;

        case num_compare:
            switch (token->state)
            {
                case S_STR:
                    arg1 = make_const(token);
                    break;

                case S_ID:
                case S_ID_FULL:
                    arg1 = find_var(token->string_data.data);
                    if(arg1 == NULL) return SEM_ERR;

                    if(arg1->type != variable_string) return TYPE_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                    return TYPE_ERR;

                default:
                    return SYNTAX_ERR;
            }
            // STRING s1 is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            //expected ','
            if (token->state != S_COMMA) return SYNTAX_ERR;
            //token is covered , so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //expected String s2
            switch (token->state)
            {
                case S_STR:
                    arg2 = make_const(token);
                    break;

                case S_ID:
                case S_ID_FULL:
                    arg2 = find_var(token->string_data.data);
                    if(arg2 == NULL) return SEM_ERR;

                    if(arg2->type != variable_string) return TYPE_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                    return TYPE_ERR;

                default:
                    return SYNTAX_ERR;
            }
            // STRING s2 is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                //when is ,
                case S_COMMA:
                    if (get_next_token(token) == LEX_ERR) return LEX_ERR;

                    switch (token->state)
                    {
                        case S_ID:
                        case S_ID_FULL:
                            var1 = find_var(token->string_data.data);
                            if(var1 == NULL) return SEM_ERR;

                        case S_INT:
                        case S_DOUBLE:
                        case S_STR:
                            return TYPE_ERR;

                        default:
                            return SYNTAX_ERR;
                    }

                //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            // ) is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, arg1, arg2, help_var, I_COMPARE);

            return  OK;

        case num_find:
            switch (token->state)
            {
                case S_STR:
                    arg1 = make_const(token);
                    break;

                case S_ID:
                case S_ID_FULL:
                    arg1 = find_var(token->string_data.data);
                    if(arg1 == NULL) return SEM_ERR;

                    if(arg1->type != variable_string) return TYPE_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                    return TYPE_ERR;

                default:
                    return SYNTAX_ERR;
            }
            // STRING s is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            //expected ','
            if (token->state != S_COMMA) return SYNTAX_ERR;
            //token is covered , so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //expected String search
            switch (token->state)
            {
                case S_STR:
                    arg2 = make_const(token);
                    break;

                case S_ID:
                case S_ID_FULL:
                    arg2 = find_var(token->string_data.data);
                    if(arg2 == NULL) return SEM_ERR;

                    if(arg2->type != variable_string) return TYPE_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                    return TYPE_ERR;

                default:
                    return SYNTAX_ERR;
            }
            // STRING search is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                    //when is ,
                case S_COMMA:
                    if (get_next_token(token) == LEX_ERR) return LEX_ERR;

                    switch (token->state)
                    {
                        case S_ID:
                        case S_ID_FULL:
                            var1 = find_var(token->string_data.data);
                            if(var1 == NULL) return SEM_ERR;

                        case S_INT:
                        case S_DOUBLE:
                        case S_STR:
                            return TYPE_ERR;

                        default:
                            return SYNTAX_ERR;
                    }

                    //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            // ) is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, arg1, arg2, help_var, I_FIND);

            return  OK;

        case num_sort:
            switch (token->state)
            {
                case S_STR:
                    arg1 = make_const(token);
                    break;

                case S_ID:
                case S_ID_FULL:
                    arg1 = find_var(token->string_data.data);
                    if(arg1 == NULL) return SEM_ERR;

                    if(arg1->type != variable_string) return TYPE_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                    return TYPE_ERR;

                default:
                    return SYNTAX_ERR;
            }
            // STRING s is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            switch (token->state)
            {
                //it is expected
                case S_RTHE:
                    break;

                    //when is ,
                case S_COMMA:
                    if (get_next_token(token) == LEX_ERR) return LEX_ERR;

                    switch (token->state)
                    {
                        case S_ID:
                        case S_ID_FULL:
                            var1 = find_var(token->string_data.data);
                            if(var1 == NULL) return SEM_ERR;

                        case S_INT:
                        case S_DOUBLE:
                        case S_STR:
                            return TYPE_ERR;

                        default:
                            return SYNTAX_ERR;
                    }

                    //otherwise is SYNTAX_ERR
                default:
                    return SYNTAX_ERR;
            }

            // ) is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            generate_instruction(INS_LIST, arg1, NULL, help_var, I_SORT);

            return  OK;


        default:
            //clear arguments
            fun->act_arg = 0;

            result = params(fun);    //go into <params>
            if (result != OK) return result;

            //expected )
            if (token->state != S_RTHE) return SYNTAX_ERR;
            // ) is covered o next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
    }

    if(fun->act_arg != fun->args_count)
    {
        fprintf(stderr, "Few or more arguments of call function: %s \n", fun->name);
        return TYPE_ERR;
    }



    //call function
    generate_instruction(INS_LIST, fun, NULL, NULL, I_CALL);

    //save return value
    if(var != NULL)
    {
        generate_instruction(INS_LIST, NULL, NULL, var, I_ASS_RET);
    }

    return OK;
}




int params(symbol_function * fun)   //item = called function
{

    int result;

    switch (token->state)
    {
        //28: <params> 	-> <value> <more_param>
        case S_ID:
        case S_ID_FULL:
        case S_INT:
        case S_DOUBLE:
        case S_STR:

            result = value(fun);    //go into <value>
            if (result != OK) return result;

            result = more_param(fun);    //go into <more_param>
            if (result != OK) return result;

            return OK;

            //29: <params> 	-> e
        case S_RTHE:
            //end of params
            //printf(" -> -> called function '%s' without any param!\n", fun->name); //debug

            return OK;

        default:
            print_error("params");
            return SYNTAX_ERR;
    }
}


int more_param(symbol_function * fun)
{
    int result;

    switch (token->state)
    {
        //34: <more_param> -> , <value> <more_param>
        case S_COMMA:
            //comma is cover, so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = value(fun);    //go into <value>
            if (result != OK) return result;

            result = more_param(fun);    //go into <more_param>
            if (result != OK) return result;
            return OK;

            //35: <more_param>	-> e
        case S_RTHE:
            return OK;


        default:
            print_error("more_param");
            return SYNTAX_ERR;
    }
}






int value(symbol_function * function)
{
    symbol_variable * var;
    function->act_arg = function->act_arg + 1;


    switch (token->state)
    {
        //42: <value> 	-> integer
        case S_INT:
            var = make_const(token);

            //instruction for add arg to call function
            generate_instruction(INS_LIST, var, NULL, &function->args[function->act_arg-1], I_ASSIGN);

            //so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

            //43: <value> -> decimal
        case S_DOUBLE:
            var = make_const(token);

            generate_instruction(INS_LIST, var, NULL, &function->args[function->act_arg-1], I_ASSIGN);
            //so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

            //44: <value> 	-> text
        case S_STR:
            var = make_const(token);

            generate_instruction(INS_LIST, var, NULL, &function->args[function->act_arg-1], I_ASSIGN);
            //so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        case S_ID:
            var = find_var(token->string_data.data);
            if(var == NULL)
            {
                fprintf(stderr, "No such variable: '%s'\n",token->string_data.data );
                return SEM_ERR;
            }

            generate_instruction(INS_LIST, var , NULL, &function->args[function->act_arg-1], I_ASSIGN);

            //so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        case S_ID_FULL:
            var = find_var(token->string_data.data);
            if(var == NULL)
            {
                fprintf(stderr, "No such variable: '%s'\n",token->string_data.data );
                return SEM_ERR;
            }

            generate_instruction(INS_LIST, var , NULL, &function->args[function->act_arg-1], I_ASSIGN);

            //so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        default:
            print_error("value");
            return SYNTAX_ERR;
    }
}


int cat()
{
    int result;
    symbol_variable * term;

    switch (token->state)
    {
        //32: <cat> -> + <value> <cat>
        case S_ADD:
            //add is cover, so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            switch (token->state)
            {
                case S_ID:
                case S_ID_FULL:
                    term = find_var(token->string_data.data);
                    if(term == NULL) return SEM_ERR;
                    break;

                case S_INT:
                case S_DOUBLE:
                case S_STR:
                    term = make_const(token);
                    break;

                default:
                    return SYNTAX_ERR;
            }

            generate_instruction(INS_LIST, term, NULL, NULL, I_PRINT);

            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = cat();    //go into <cat>
            if (result != OK) return result;

            return OK;

            //33: <cat>	-> e
        case S_RTHE:
            return OK;

        default:
            print_error("cat");
            return SYNTAX_ERR;
    }
}
