/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef SYM_TABLE_H
#define SYM_TABLE_H



#include "error.h"
#include "scaner.h"
#include "stack.h"
#include "ial.h"
#include "generate.h"

typedef bst_node *tSymbolTable;

typedef struct type_with_id{
    int id_type;
    char * name;
}tType_id;

//struct for parser position
typedef struct where_parser {
    char * class;
    bst_node * class_ptr;
    char * fun;
    bst_node * fun_ptr;
} TParser_pos;

extern TParser_pos Parser_position;
extern t_token * token;
extern tSymbolTable * Glob_tab_sym;
extern ins_list * INS_LIST; // actual list of instruction -> use it
extern ins_list * Glob_INS_LIST; //global list of instruction -> for parser

/* Main function -> parser and expressions use*/

//function for search var, only with the name. (param is id_full or id)
//return: NULL if no such variable in loc or Glob, or pointer to symbol_variable -> describe in symbol.h
symbol_variable * find_var(char * name);


//function for search fun, only with the name. (param is id_full or id)
//return: NULL if no such function, or pointer to symbol_function -> describe in symbol.h
symbol_function * find_fun(char * name);


//function for testing, if the id is function (param is id_full or id)
//return TRUE or FALSE
int is_function(char * name);


//function for testing, if the id is variable (param is id_full or id)
//return TRUE or FALSE
int is_variable(char * name);


//function to check if var or fun exist (param is id_full or id)
//return TRUE or FALSE
int check_exist(char * name);

/*END of MAIN functions*/



//insert new_class -- and set the Global Parser_position
int new_class(char * class_name);

//set class to  Global Parser_position
int set_class(char * class_name);

//insert new_function to global table of Parser_position.class
int new_function(tType_id fun);

//set function to Global Parser_position
int set_function(tType_id fun);

//unset function to Global Parser_position
void unset_function();

//insert variable to global table of Parser_position.class
int new_glob_variable(tType_id var);

//insert new local variable to table of Parser_position.fun
int new_loc_variable(tType_id loc);

//insert new arg to func in Position_parser
int add_arg(tType_id arg);

//function return true or false
int is_function(char * name);



//print class and their table
void print_class();








//return 1, or 0 -> if the name is full id 1
int is_full_id(char * name);

//help function to copy string
void copy_name(char ** dst, char * src);

//help function to code data type for IAL.h
variable_type code_type(int type);


//helped function, which split id_full into 2 string
void split_full_id(char ** class, char ** fun, char * id_full);

#endif //SYM_TABLE_H
