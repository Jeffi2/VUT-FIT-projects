/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef expressions_h
#define expressions_h

#include <stdbool.h>
#include "scaner.h"
#include "parser.h"
#include "sym_table.h"
#include "generate.h"
#include "constants.h"
// global variables token and instruction list
extern ins_list *INS_LIST;
extern  t_token *token;

//type for oprarations
typedef enum{
    O_ADD=0, //0
    O_SUB,  //1
    O_MUL,  //2
    O_DIV,  //3
    O_LESS, //4
    O_MORE, //5
    O_LESS_EQUAL,   //6
    O_MORE_EQUAL,   //7
    O_EQUAL,    //8
    O_NOT_EQUAL,//  9
    O_ID,   //10
    O_EMPTY, //11
    O_LTH, //12
    O_RTH, //13
    O_CONST=15,//15
    O_STOP=-1,//start of handle
    E=14 // nonterminal but is only 1
}expr_term;



typedef enum{
    SHIFT,
    REDUCE,
    ERROR,
    EQUAL,
    END_EXPRESSION
}t_action;

typedef struct p_item  {
    bool term; //is term
    t_token * token; //space for copied token
    symbol_variable * data; //data for interpret
    expr_term operator; //operator
}T_ptable_item;

/*
 *
 */
typedef struct pstack{
    unsigned last_term; // toppiest terminal
    unsigned top; // top of stack -here we can write
    unsigned size; // alocated size of stack
    struct p_item  item[]; //flexible array member array structs p_item
}t_pstack;

//t_pstack *pstack; I dont know but I thing we dont need id :)

int expr_stack_init(t_pstack** expr_stack);
/*
 * inicializace zasobniku na na nulove hodnoty
 * alokace vychozi velikosti na 100
 */
expr_term expr_translate(t_state operator,int* err);
/*
 * preklada znak z tokenu na adekvatni operator
 * pro precedencni tabulku
 */
int expr_make_place(t_pstack** expr_stack, unsigned position);
/*
 * vytvoří místo na zasobniku pro vložení jednoho prvku na
 * pozici position
 */
int expr_stack_realloc(t_pstack** expr_stack);

t_action expr_get_rule(t_pstack** expr_stack,expr_term operator);
/*
 * vrati hodnotu nalezenou na pozici x y v prrecedencni tabulce
 */

int expr_push_top(t_pstack** expr_stack, expr_term operator);
/*
 * push operator on top of stack
 *
 * expr_stack pointer on stack
 * operator comming operator from scanner
 *
 * return 1 if succes
 * return -1 if error occured
 */
int expr_check_size(t_pstack** expr_stack);
/*
 * expr_stack pointer on stack
 *  check if we need realloc and do this
 *
 *  return 1 if succes
 *  return -1 on fail
 */

int expr_push_pos(t_pstack** expr_stack, expr_term operator,unsigned pos);
/*
 * push operator on pos into stack expr_stack
 *
 * return 1 if succes else return -1
 */
void expr_print_stack(t_pstack** expr_stack);
/*
 * debugging function print convert number representation of stac to chars
 * and print it to stdout
 */

int expr_reduce(t_pstack** expr_stack);
/*
 * first check which rule we use to reduce
 * on position of topmost get nonterminal E
 * move top of stack and change position of last termial
 *
 * return 0 if succes
 * return -1 if we can check position outside of array
 *
 * exit if occure some unexpected input
 */
int expressions(symbol_variable ** last_p);
/*
 * Starting function , this call parser if reach start of expression
 *
 * return OK if all is clear
 * return ERR_INTERNAL if fail malloc realooc or same interial error come
 *
 * call exit if fatal error come (unexpected input) before exit print on stderr message
 *
 */

#endif
