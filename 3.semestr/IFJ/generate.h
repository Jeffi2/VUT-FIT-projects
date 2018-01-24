/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef generate_h
#define generate_h

#include "string.h"
#include "stack.h"
#include "error.h"
#include "symbol.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
    I_NOT, //0 negation of logical result /true -> /false [adr1 - operand, adr2 - NULL, adr3 - vysledek]
    
    //aritmec
    I_ADD, // + [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek -> a = b + c]
    I_SUB, //2 - [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek -> a = b - c]
    I_MUL, //3 * [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek -> a = b * c]
    I_DIV, //4 / [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek -> a = b / c]
    
    //compare
    I_LESS, //5 < [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek(in int value) -> a = b < c]
    I_GREAT,//6 > [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek(in int value) -> a = b > c]
    I_LESSQ,//7 <= [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek(in int value) -> a = b <= c]
    I_GREATQ,//8 >= [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek(in int value) -> a = b >= c]
    I_EQ, //9 == [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek(in int value) -> a = b == c]
    I_NOTEQ,//10 != [adr1 - prvni operand, adr2 - druhy operand, ard3 - vysledek(in int value) -> a = b != c]
    
    //clasic
    I_ASSIGN,//11 = [adr1 - co se prirazuje, adr2 - NULL, adr3 - kam]
    I_LABEL, //12 navesti [generate as a new list item in parser]
    I_CALL, //13 call function [adr1 - pointer to symbol_function struct, adr2 - NULL, adr3 - NULL]
    I_RETURN, //14 return from function [adr1 - vysledek or NULL, adr2 - pointer to bst_node, adr3 - NULL]
    I_GOTO, //15 goto in condition or loop [adr1 - condition or NULL(hard jump), adr2 - NULL, adr3 - label to jump]
    
    //build in function
    I_READ_INT, //16 readInt [adr1 - NULL, adr2 - NULL, adr3 - read int value]
    I_READ_D, //17 readDouble [adr1 - NULL, adr2 - NULL, adr3 - read double value]
    I_READ_STR, //18 readString [adr1 - NULL, adr2 - NULL, adr3 - read string value]
    I_PRINT, //19 write [adr1 - whatever to be print, adr2 - NULL, adr3 - NULL]
    I_LENGTH, //20 str lenght [adr1 - String s, adr2 - NULL, adr3 - returned int value]
    I_SUBSTR, //21 str substr [adr1 - pointer to symbol_variable array, adr2 - NULL, adr3 - vysledek]
    I_COMPARE, //22 str compare [adr1 - String s1, adr2 - String s2, ade3 - returned int value]
    I_FIND, //23 str find [adr1 - String s, adr2 - String search, ade3 - returned int value]
    I_SORT, //24 sort in string [adr1 - String s, adr2 - NULL, adr3 - returned String]
    I_HALT, //25 - HALT!!!!
    I_ASS_RET, // 26 assign return[adr1 - NULL, adr2 - NULL, adr3 - symbol variable]
} t_ins;

//instruction detail
typedef struct ins{
    int type;
    void *addr1;
    void *addr2;
    void *addr3;
} instruction;

//item of instruction list
typedef struct ins_item{
    instruction ins;
    struct ins_item *next;
    struct ins_item *prev;
} *ins_item_ptr;

//instruction list
typedef struct instruction_list{
    ins_item_ptr first;
    ins_item_ptr last;
    ins_item_ptr act;
} ins_list;

void init_list(ins_list *);
void dispose_list (ins_list *);
void insert_first (ins_list *, void *, void *, void *, int);
void insert_last (ins_list *, void *, void *, void *, int);
void first (ins_list *);
void last (ins_list *);
void delete_first (ins_list *);
void succ (ins_list *);
void generate_instruction (ins_list *list, void *, void *, void *, int);
ins_list* create_fnc_list();
ins_item_ptr make_label(); //return pointer to inst label field
void insert_label(ins_item_ptr label_field, ins_list *i_list);
void print_ins_list_debug(ins_list *list);



#endif /* generate_h */

