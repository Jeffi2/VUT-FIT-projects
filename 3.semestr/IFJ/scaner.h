/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#ifndef scaner_h
#define scaner_h

#include "string.h"
#include "stack.h"
#include "error.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


typedef struct Ttoken {
    int state;
    int int_data;
    double double_data;
    t_String string_data;
} t_token;

typedef enum {
    S_EOF, //0-> end of file
    
    S_ID, //1-> simple identificator
    S_ID_FULL, //2-> ID.method
    S_INT, //3-> int number
    S_DOUBLE, //4-> 1.1
    S_STR,  //5-> inner string
    
    S_DIV, //6-> /
    S_ADD, //7-> +
    S_SUB, //8-> -
    S_MUL, //9-> *
    S_SEMICOLON, //10-> ;
    S_LTHE, //11-> (
    S_RTHE, //12-> )
    S_LCUR, //13-> {
    S_RCUR, //14-> }
    S_COMMA, //15-> ,
    S_ASSIGN, //16-> =
    S_EQUAL, //17-> ==
    S_LESS, //18-> <
    S_MORE, //19-> >
    S_LESS_EQUAL, //20-> <=
    S_MORE_EQUAL, //21-> >=
    S_NOT, //22-> !
    S_NOT_EQUAL, //23-> !=
    //KEYWORDS
    S_BOOL, //24
    S_BREAK, //25
    S_CLASS, //26
    S_CONTINUE, //27
    S_DO, //28
    S_DOUBLE_KEY, //29
    S_ELSE, //30
    S_FALSE, //31
    S_FOR, //32
    S_IF, //33
    S_INT_KEY, //34
    S_RETURN, //35
    S_STRING, //36
    S_STATIC, //37
    S_TRUE, //38
    S_VOID, //39
    S_WHILE, //40
    S_ERROR, //41-> error in lexical analysis
    
    S_DOUBLE_CH, //42-> 1. (check state)
    S_START, //43-> start
    S_ID_CH, //44-> ID. (check state)
    S_EXP_CH, //45-> 1.1E 1.1e (check state)
    S_EXP_CH2, //46-> 1.1E+ 1.1e- (check state)
    S_EXP_CH3, //47-> 1.1e9 1.1E+9 1.1e-2
    S_ESC_CH, //48-> escape (\ check state)
    S_ESC_CH1, //49-> \1 (check state - valid esc)
    S_ESC_CH2, //50-> \12 (check state - valid esc)
  //  S_ESC_CH3, //51-> \0 (check state - valid esc)
  //  S_ESC_CH4, //52-> \01 (check state - valid esc)
    S_COM_LINE, //51-> //
    S_COM_BLOCK1, //52-> /*
    S_COM_BLOCK2, //53-> /*******   (/)-> go to start
} t_state;


//chang vlada, because error in build
extern t_token hang_token[3];

int get_next_token(t_token *token);
void set_file(FILE *file);
t_token* token_init();
void clean_token (t_token *token);
int str_to_int (t_token* token);
int str_to_double (t_token* token);
void push_char_back(int c);
void keywords_check(t_token* token);
void hang_array_init ();
void hang_pop(t_token *token);
void push_token_back();
void full_token_copy(t_token *first, t_token *second);
void half_token_init(t_token *token);

#endif /* scaner_h */
