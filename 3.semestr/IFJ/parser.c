/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "parser.h"

#define FALSE 0
#define TRUE 1

int parse(tSymbolTable * ST)
{
    int result;
    //init tabel of symbol
    Glob_tab_sym = ST;

    // TOKEN INIT DO MALLOC FIRST STRING!
    token = token_init();

    //init string to token
    Stack_push(&Garbage, token);

    //add to table of symbol built_in function and class for control
    if (First_look)
    {
        result = add_built_in();
        if(result != OK) return result;

    }
    else
    {
        //because static declaration is in global instruction
        INS_LIST = Glob_INS_LIST;
    }

    if (get_next_token(token) == LEX_ERR)
        result = LEX_ERR; // Error lexical analyzer
    else
        result = prog();

    //free token->string
    Stack_push(&Garbage, token->string_data.data);


    if(result != OK && !First_look)
    {
        fprintf(stderr,"LAST token->state = %d\n", token->state);

        if (token->state == S_ID || token->state == S_ID_FULL || token->state == S_STR)
            fprintf(stderr, "Error: %s !!\n", token->string_data.data);
    }

    return result;

}


void check(void * item)
{
    if(item == NULL)
    {
        fprintf(stderr, "Error in add_built_in - not found\n");
        exit(ERR_INTERNAL);
    }
}

int add_built_in()
{
    int result;

    //insert class ifj16
    result = new_class("ifj16");
    if(result != OK) return result;

    tType_id fun;

    //insert function int readInt();
    fun.name = "readInt";
    fun.id_type = S_INT_KEY;
    result = new_function(fun);
    if(result != OK) return result;

    symbol_function * function = find_fun(fun.name);
    check(function);
    function->defined = num_readInt;


    //insert function double readDouble();
    fun.name = "readDouble";
    fun.id_type = S_DOUBLE_KEY;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_readDouble;

    //insert function String readString();
    fun.name = "readString";
    fun.id_type = S_STRING;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_readString;


    //insert function void print(term or cat);
    fun.name = "print";
    fun.id_type = S_VOID;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_print;

    //insert function int length(String s)
    fun.name = "length";
    fun.id_type = S_INT_KEY;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_length;

    //insert function String substr(String s , int i , int n )
    fun.name = "substr";
    fun.id_type = S_STRING;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_substr;


    //insert params
    tType_id arg;
    arg.name = "s";
    arg.id_type = S_STRING;
    result = add_arg(arg);
    if (result != OK)
        return result;

    arg.name = "i";
    arg.id_type = S_INT_KEY;
    result = add_arg(arg);
    if (result != OK)
        return result;

    arg.name = "n";
    arg.id_type = S_INT_KEY;
    result = add_arg(arg);
    if (result != OK)
        return result;


    //insert function int compare(String s1 , String s2 )
    fun.name = "compare";
    fun.id_type = S_INT_KEY;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_compare;

    //insert function int find(String s , String search )
    fun.name = "find";
    fun.id_type = S_INT_KEY;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_find;

    //insert function String sort(String s )
    fun.name = "sort";
    fun.id_type = S_STRING;
    result = new_function(fun);
    if(result != OK) return result;

    function = find_fun(fun.name);
    check(function);
    function->defined = num_sort;
    return OK;
}


int has_main_run()
{
    symbol_function *fun_run;

    fun_run = find_fun("Main.run");
    if(fun_run == NULL)
        return FALSE;

    if(fun_run->args_count != 0)
        return FALSE;

    if(fun_run->return_type != variable_void)
        return FALSE;

    return TRUE;
}