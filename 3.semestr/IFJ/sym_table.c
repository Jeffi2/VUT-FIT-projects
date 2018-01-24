/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "sym_table.h"

#define TRUE 1
#define FALSE 0

tSymbolTable * Glob_tab_sym;
t_token * token;     //global token;
TParser_pos Parser_position;
ins_list * INS_LIST; // actual list of instruction
ins_list * Glob_INS_LIST; //global list of instruction

int new_class(char * class_name)
{
    if (bst_insert_class(Glob_tab_sym, class_name) != 0) return SEM_ERR;

    bst_node * item  = bst_node_find(*Glob_tab_sym, class_name);
    if (item == NULL) return ERR_INTERNAL;

    Parser_position.class = item->key;
    Parser_position.class_ptr = item;

    Parser_position.fun = NULL;
    Parser_position.fun_ptr = NULL;

    //printf("New Class: %s : %p\n", Parser_position.class, (void *) Parser_position.class_ptr); //debug

    return OK ;

}

int set_class(char * class_name)
{
    bst_node * item  = bst_node_find(*Glob_tab_sym, class_name);
    if (item == NULL) return ERR_INTERNAL;

    Parser_position.class = item->key;
    Parser_position.class_ptr = item;

    Parser_position.fun = NULL;
    Parser_position.fun_ptr = NULL;

    //printf("Set Class: %s : %p\n", Parser_position.class, (void *) Parser_position.class_ptr); //debug

    return OK;
}


int new_function(tType_id fun)
{
    //todo -> function offset
    if (bst_insert_function(&(Parser_position.class_ptr->ptr.global),fun.name, code_type(fun.id_type)) != OK) return SEM_ERR;

    bst_node * item  = bst_node_find(Parser_position.class_ptr->ptr.global, fun.name);
    if (item == NULL) return ERR_INTERNAL;


    Parser_position.fun = item->key;
    Parser_position.fun_ptr = item;

    //printf("\tNew Function: %s : %p\n", Parser_position.fun, (void *) Parser_position.fun_ptr); //debug

    return OK;
}

int set_function(tType_id fun)
{
    bst_node * item  = bst_node_find(Parser_position.class_ptr->ptr.global, fun.name);
    if (item == NULL) return ERR_INTERNAL;


    Parser_position.fun = item->key;
    Parser_position.fun_ptr = item;

    //printf("\tSet Function: %s : %p\n", Parser_position.fun, (void *) Parser_position.fun_ptr); //debug

    return OK;
}

void unset_function()
{

    //printf("\t Unset Function %s : %p\n", Parser_position.fun, (void *) Parser_position.fun_ptr); //debug

    Parser_position.fun = NULL;
    Parser_position.fun_ptr = NULL;


}

int new_glob_variable(tType_id var)
{
    if (bst_insert_variable(&(Parser_position.class_ptr->ptr.global),var.name, code_type(var.id_type)) != OK) return SEM_ERR;

    //TODO -> optimze for speed
    bst_node * item  = bst_node_find(Parser_position.class_ptr->ptr.global, var.name);
    if (item == NULL) return ERR_INTERNAL;


    //printf("\tNew Global Variable: %s : %p\n", item->key, (void *) item); //debug
    return OK;
}

int new_loc_variable(tType_id var)
{
    if (bst_insert_variable(&(Parser_position.fun_ptr->ptr.function->local_table),var.name, code_type(var.id_type)) != OK) return SEM_ERR;

    //TODO -> optimze for speed
    bst_node * item  = bst_node_find(Parser_position.fun_ptr->ptr.function->local_table, var.name);
    if (item == NULL) return ERR_INTERNAL;


    //printf("\t\tNew Local Variable: %s : %p\n", item->key, (void *) item); //debug
    return OK;
}


int add_arg(tType_id arg)
{
    if (bst_node_add_arg(Parser_position.fun_ptr, arg.name, code_type(arg.id_type)) != OK) return SEM_ERR;

    //todo -> optimize for speed
    bst_node * item  = bst_node_find(Parser_position.fun_ptr->ptr.function->local_table, arg.name);
    if (item == NULL) return ERR_INTERNAL;

    //printf("\t\t -> New arg of function: %s, name: %s\n", Parser_position.fun, item->key); //debug

    return OK;
}

void print_class()
{
    bst_inorder(*Glob_tab_sym, "");
}


int check_exist(char * name)
{
    return (is_variable(name) || is_function(name));
}



/* Work with FUNCTION*/

symbol_function * find_fun(char * name)
{

    bst_node * item = NULL;

    if (is_full_id(name))
    {
        char * class;
        char * fun;

        //todo !! -> check built in function !!!

        split_full_id(&class, &fun, name);

        //find class
        bst_node * class_ptr  = bst_node_find(*Glob_tab_sym, class);
        if (class_ptr == NULL) return NULL;

        //find function
        item  = bst_node_find(class_ptr->ptr.global, fun);
    }
    else
    {
        //fun in class where is parser
        //find function
        item  = bst_node_find(Parser_position.class_ptr->ptr.global, name);
    }

    if (item == NULL) return NULL;

    //check it is fun?
    if (item->type == type_function)
        return item->ptr.function;

    return NULL;

}

int is_function(char * name)
{
    void * result = (void *) find_fun(name);
    if ( result == NULL)
        return FALSE;

    return TRUE;
}

/* END of wokr with FUNCTION*/

/* WORK with VARIABLE*/

symbol_variable* find_var(char * name)
{
    bst_node * item = NULL;

    if (is_full_id(name))
    {
        //is global var

        char * class;
        char * var;

        split_full_id(&class, &var, name);

        //find class
        bst_node * class_ptr  = bst_node_find(*Glob_tab_sym, class);
        if (class_ptr == NULL) return NULL;

        //find function
        item  = bst_node_find(class_ptr->ptr.global, var);
    }
    else
    {
        if(Parser_position.fun_ptr != NULL)
        {
            //first search in local tree:
            item = bst_node_find(Parser_position.fun_ptr->ptr.function->local_table, name);
        }

        if(item == NULL) //it isnt local variable -> so find in global
        {
            item = bst_node_find(Parser_position.class_ptr->ptr.global, name);
        }
    }

    if (item == NULL) return NULL;

    //check it is var?
    if (item->type == type_variable)
        return item->ptr.variable;

    return NULL;
}

int is_variable(char * name)
{
    void * result = (void *) find_var(name);
    if ( result == NULL)
        return FALSE;

    return TRUE;
}

/* END of work with VARIABLE*/




/*  Minority function -> only for help -^ */


void split_full_id(char ** class, char ** fun, char * id_full)
{
    *class = malloc(strlen(id_full) + 1);
    *fun = malloc(strlen(id_full) + 1);

    Stack_push(&Garbage, *class);
    Stack_push(&Garbage, *fun);

    int i = 0;

    while(id_full[i] != '.')
    {
        (*class)[i] = id_full[i];
        i++;
    }

    (*class)[i] = '\0';
    i++;

    int j = 0;

    while(id_full[i] != '\0')
    {
        (*fun)[j] = id_full[i];

        j++;
        i++;
    }

    (*fun)[j] = '\0';
}

void copy_name(char ** dst, char * src)
{
    *dst = malloc(strlen(src) + 1);
    if(*dst == NULL)
    {
        fprintf(stderr, "Error in copy_name in malloc !!\n");
        exit(ERR_INTERNAL);
    }

    strcpy(*dst, src);
}

variable_type code_type(int type)
{
    switch(type)
    {
        case S_INT_KEY:
            return variable_integer;

        case S_DOUBLE_KEY:
            return variable_double;

        case S_STRING:
            return variable_string;

        case S_VOID:
            return variable_void;

        default:
            fprintf(stderr, "Error on code_type - unknown type: %d\n", type);
            exit(ERR_INTERNAL);
    }
}


int is_full_id(char * name)
{
    int i = 0;

    while(name[i] != '\0')
    {
        if(name[i] == '.')
            return TRUE;

        i++;
    }

    return FALSE;
}
