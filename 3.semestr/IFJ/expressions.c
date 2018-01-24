/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "expressions.h"
#include "stack.h"
#include "error.h"
#include "scaner.h"
#include "generate.h"
#include "interpret.h"
#include "symbol.h"

#define MAKE_E     (*expr_stack)->item[(*expr_stack)->top-4].operator=E
#define CLEAN_STACK \
                    for (int i=1;i<3;i++) {\
                    (*expr_stack)->item[(*expr_stack)->top - i].token = NULL;\
                    (*expr_stack)->item[(*expr_stack)->top - i].data = NULL;\
                    (*expr_stack)->item[(*expr_stack)->top - i].term = true;\
                    }

#define GENERATE(x) \
                    tmp_symbol = make_help_var();\
                    (*expr_stack)->item[(*expr_stack)->top-4].data=tmp_symbol;\
                    generate_instruction(INS_LIST, (*expr_stack)->item[(*expr_stack)->top-3].data,(*expr_stack)->item[(*expr_stack)->top-1].data, tmp_symbol, x);

#define MOVE_TOP \
                    (*expr_stack)->last_term=(*expr_stack)->last_term-3;\
                    (*expr_stack)->top-=3;


extern Stack Garbage;
//init array for precedence table
const char* precedence_table[]={
        //              entering token
        //    0   1   2   3   4   5  6   7   8   9   10  11  12  13
        //   +   -   *   /   <   >  <=  >=  ==  !=  ID   $  (    )
/* +  0 */   ">" ">" "<" "<" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* -  1 */   ">" ">" "<" "<" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* *  2 */   ">" ">" ">" ">" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* /  3 */   ">" ">" ">" ">" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* <  4 */   "<" "<" "<" "<" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* >  5 */   "<" "<" "<" "<" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">", //last changed
/* <= 6 */   "<" "<" "<" "<" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* >= 7 */   "<" "<" "<" "<" ">" ">" ">" ">" ">" ">" "<" ">" "<" ">",
/* == 8 */   "<" "<" "<" "<" "<" "<" "<" "<" ">" ">" "<" ">" "<" ">",
/* != 9 */   "<" "<" "<" "<" "<" "<" "<" "<" ">" ">" "<" ">" "<" ">",
/* ID 10*/   ">" ">" ">" ">" ">" ">" ">" ">" ">" ">" "#" ">" "#" ">",
/* $  11*/   "<" "<" "<" "<" "<" "<" "<" "<" "<" "<" "<" "#" "<" "?",
/* (  12*/   "<" "<" "<" "<" "<" "<" "<" "<" "<" "<" "<" "#" "<" "=",
/* )  13*/   ">" ">" ">" ">" ">" ">" ">" ">" ">" ">" "#" ">" "#" ">",
};

expr_term expr_translate(t_state operator,int *err){
    *err=0;

    switch (operator)
    {
        case S_SUB:
            return O_SUB;
        case S_ADD:
            return O_ADD;
        case S_MUL:
            return O_MUL;
        case S_DIV:
            return O_DIV;
        case S_LESS:
            return O_LESS;
        case S_MORE:
            return O_MORE;
        case S_LESS_EQUAL:
            return O_LESS_EQUAL;
        case S_MORE_EQUAL:
            return O_MORE_EQUAL;
        case S_EQUAL:
            return O_EQUAL;
        case S_NOT_EQUAL:
            return O_NOT_EQUAL;
        case S_ID:
            return O_ID;
        case S_LTHE:
            return O_LTH;
        case S_RTHE:
            return O_RTH;
        case S_INT:
            return O_CONST;
        case S_ID_FULL:
            return O_ID;
        case S_DOUBLE:
            return O_CONST;
        case S_STR:
            return O_CONST;
        case S_EOF:
            return O_EMPTY;
        default:
            break;

    }
    *err=-1;
    return 1;
}
int expr_stack_init(t_pstack** expr_stack){
//init defaul values in stack and malloc 100 items for expressions
    *expr_stack=malloc(sizeof(struct pstack)+sizeof(struct p_item)*100);
    if (*expr_stack == NULL)
        return -1;

    (*expr_stack)->top=1;
    (*expr_stack)->last_term=0;
    (*expr_stack)->size=100;
    (*expr_stack)->item[0].token=NULL;
    (*expr_stack)->item[0].operator=O_EMPTY;
    (*expr_stack)->item[0].term=true;


    return 1;

}
int expr_stack_realloc(t_pstack** expr_stack){
    //if we need more space then 100 items in stack for expression so we call realloc
    if ((*expr_stack) == NULL)
        return -1;
    *expr_stack=realloc(*expr_stack,(sizeof(struct pstack)+sizeof(struct p_item)*(((*expr_stack)->size)+50)));
    if ((*expr_stack) == NULL)
        return -1;
    (*expr_stack)->size+=50;
    return 1;
}

int expr_check_size(t_pstack** expr_stack){

    if ((*expr_stack)->size == (*expr_stack)->top+1)
        if(expr_stack_realloc(*&expr_stack) == -1)
            return -1;
    return 1;
}

int expr_push_top(t_pstack** expr_stack, expr_term operator){

    if (expr_check_size(*&expr_stack) != 1)
        return -1;

    (*expr_stack)->item[(*expr_stack)->top].operator=operator;
    (*expr_stack)->item[(*expr_stack)->top].term=true;
    if (operator!=E)
        (*expr_stack)->last_term=(*expr_stack)->top;

    (*expr_stack)->top++;

    return 1;
}
int expr_push_pos(t_pstack** expr_stack, expr_term operator, unsigned pos){

    int state;

    if (pos == (*expr_stack)->top) {
        //if position is same like top call push top
        if (expr_push_top(&*expr_stack, operator))

            return 1;
    }

    if (pos < (*expr_stack)->top) {
        //check if we need place for smt
        state = expr_make_place(expr_stack, pos);
    }
    if (state == -1)
        return -1;

    if (expr_check_size(*&expr_stack) != 1)
        return -1;


    (*expr_stack)->item[pos].operator = operator;
    (*expr_stack)->item[pos].term = true;

    if ((*expr_stack)->last_term < pos)
        (*expr_stack)->last_term = pos;

    return 1;
}

int expr_make_place(t_pstack** expr_stack, unsigned position){

//test zda neni potřeba realokace
    if ((*expr_stack)->size == (*expr_stack)->top+1)
        if(expr_stack_realloc(*&expr_stack) == -1)
            return -1;

//posouvani o jeden prvek od pozice pos
    for (unsigned i=(*expr_stack)->top; i >= position;i--){
        (*expr_stack)->item[i+1]=(*expr_stack)->item[i];
        if (i == 0)
            break;

    }
    //posunuti vrchlu zasobniku o 1
    (*expr_stack)->top++;
    (*expr_stack)->item[position].operator=O_STOP;
    return 1;
}

t_action expr_get_rule(t_pstack** expr_stack, expr_term operator){
    int tmp;
    if (operator == O_CONST)
        operator = O_ID;    //const is same like ID in this part of program
    //check end of actions with expressions
    if((*expr_stack)->item[(*expr_stack)->last_term].operator == O_EMPTY && operator == O_EMPTY)
        return END_EXPRESSION;

//search item i precedence table
    expr_term search=(*expr_stack)->item[(*expr_stack)->last_term].operator;
    //const is same like id in precedence table
    if (search == O_CONST)
        search = O_ID;
    tmp=precedence_table[search][operator];
    //decode result from precedence table

    switch (tmp)
    {
        case '>':
            return REDUCE;
        case '<' :
            return SHIFT;
        case '#':
            return ERROR;
        case '=':
            return EQUAL;
        case '?':
            return END_EXPRESSION;
        default:
            return -1;
            break;
    }

}

void expr_print_stack(t_pstack** expr_stack){
    for (unsigned int i=0;(*expr_stack)->top>i;i++)
        switch ((*expr_stack)->item[i].operator)
        { case O_ID : printf("%s","ID");
                break;
            case O_ADD:
                printf("%c",'+');
                break;
            case O_MUL:
                printf("%c",'*');
                break;
            case O_SUB:
                printf("%c",'-');
                break;
            case O_LTH:
                printf("%c",'(');
                break;
            case O_RTH:
                printf("%c",')');
                break;
            case O_DIV:
                printf("%c",'/');
                break;
            case O_STOP:
                printf("%c",'<');
                break;
            case O_EMPTY:
                printf("%c",'$');
                break;
            case E:
                printf("%c",'E');
                break;
            case O_CONST:
                printf("%c",'C');
                break;
            case O_EQUAL:
                printf("%c",'=') ;
                break;
            case O_LESS:
                printf("%s"," less ");
                break;
            case O_MORE:
                printf("%c",'>');
                break;
            default:
                printf("ID_%d  value %d\n",i,(*expr_stack)->item[i].operator);

        }
}


int expr_reduce(t_pstack** expr_stack){

    symbol_variable * tmp_symbol;


    //if top of stack is equal ID && top-1 is equal wiht start of handle e can continue
        //reduce <ID -> E
    if ((((*expr_stack)->item[(*expr_stack)->top-1].operator == O_ID)||
         ((*expr_stack)->item[(*expr_stack)->top-1].operator == O_CONST)) &&
            (*expr_stack)->item[(*expr_stack)->top-2].operator == O_STOP){
        //change item stop to E
        (*expr_stack)->item[(*expr_stack)->top-2].operator=E;


     switch ((*expr_stack)->item[(*expr_stack)->top-1].token->state)
     {
         case S_STR:

             //malloc symbol_var for interpret
             tmp_symbol = malloc(sizeof(struct Tsymbol_variable));
             if (tmp_symbol == NULL)
                 return ERR_INTERNAL;
             Stack_push(&Garbage,tmp_symbol);
             //assign type to tmp_symbolvar
             tmp_symbol->type=variable_string;
             tmp_symbol->value.value_string = NULL;
             //assign value of conts to tmp_symbol and change flag init to 1
             if (symbol_variable_assign_string(tmp_symbol,(*expr_stack)->item[(*expr_stack)->top-1].token->string_data.data) == -1)
                 return ERR_INTERNAL;
             //assign tmp_symbol to E
             Stack_push(&Garbage,tmp_symbol->value.value_string);
             (*expr_stack)->item[(*expr_stack)->top-2].data=tmp_symbol;


             break;
         case S_INT:

             //malloc symbol_var for interpret
             tmp_symbol = malloc(sizeof(struct Tsymbol_variable));
             if (tmp_symbol == NULL)
                 return ERR_INTERNAL;
             Stack_push(&Garbage,tmp_symbol);

             //assign type to tmp_symbolvar
             tmp_symbol->type=variable_integer;
             //assign value of conts to tmp_symbol and change flag init to 1
             if (symbol_variable_assign_int(tmp_symbol,(*expr_stack)->item[(*expr_stack)->top-1].token->int_data) == -1)
                 return ERR_INTERNAL;
             //assign tmp_symbol to E
             (*expr_stack)->item[(*expr_stack)->top-2].data=tmp_symbol;

             break;
         case S_DOUBLE:
             //malloc symbol_var for interpret
             tmp_symbol = malloc(sizeof(struct Tsymbol_variable));
             if (tmp_symbol == NULL)
                 return ERR_INTERNAL;
             Stack_push(&Garbage,tmp_symbol);

                //assign type to tmp_symbolvar
             tmp_symbol->type=variable_double;
             //assign value of conts to tmp_symbol and change flag init to 1
             if (symbol_variable_assign_double(tmp_symbol,(*expr_stack)->item[(*expr_stack)->top-1].token->double_data) == -1)
                 return ERR_INTERNAL;
             //assign tmp_symbol to E
             (*expr_stack)->item[(*expr_stack)->top-2].data=tmp_symbol;


             break;
         case S_ID: //S_ID S_ID_FULL need same action
         case S_ID_FULL:
             //find pointer  in varTable and assign it to reduced E
             (*expr_stack)->item[(*expr_stack)->top-2].data = find_var((*expr_stack)->item[(*expr_stack)->top-1].token->string_data.data);
            if ((*expr_stack)->item[(*expr_stack)->top-2].data == NULL)
                return SEM_ERR;
             break;

     }

        (*expr_stack)->item[(*expr_stack)->top-2].token=NULL;
        (*expr_stack)->item[(*expr_stack)->top-2].term=false;
        //clean item after reduce
        (*expr_stack)->item[(*expr_stack)->top-1].term=true;
        (*expr_stack)->item[(*expr_stack)->top-1].token=NULL;
        //change in stack infomations
        (*expr_stack)->last_term=(*expr_stack)->last_term-2;
        (*expr_stack)->top--;
        return 0;
        //describe <(E)
    }
    if ((*expr_stack)->item[(*expr_stack)->top-1].operator == O_RTH &&
        (*expr_stack)->item[(*expr_stack)->top-2].operator == E &&
        (*expr_stack)->item[(*expr_stack)->top-3].operator == O_LTH &&
        (*expr_stack)->item[(*expr_stack)->top-4].operator == O_STOP)
    {
            //change item stop(first in handle) to E nad copy data from E
            (*expr_stack)->item[(*expr_stack)->top-4].operator=E;
            (*expr_stack)->item[(*expr_stack)->top-4].data=(*expr_stack)->item[(*expr_stack)->top-2].data;
            (*expr_stack)->item[(*expr_stack)->top-4].term=false;

            // clean items -2 <->top
            for (int i=3;i>0;i--){
                (*expr_stack)->item[(*expr_stack)->top-i].term=true;
                (*expr_stack)->item[(*expr_stack)->top-i].token=NULL;

            }
            //change information about stack move top 3 items backwards on new E
            (*expr_stack)->last_term=(*expr_stack)->last_term-4; // (last - 4) ->$<(E)<-last
            (*expr_stack)->top-=3;
            return 0;
    }

    //check <E operation E

    if  ((*expr_stack)->item[(*expr_stack)->top-1].operator == E &&
         (*expr_stack)->item[(*expr_stack)->top-3].operator == E &&
         (*expr_stack)->item[(*expr_stack)->top-4].operator == O_STOP){

            switch ((*expr_stack)->item[(*expr_stack)->top-2].operator)
            {
                case O_ADD:
                    MAKE_E;             //macro which help with reduce to E
                    GENERATE(I_ADD);    //macro generating instruction x
                    CLEAN_STACK;        //macro cleaning unusefull items
                    MOVE_TOP;           //macro move information about top of vector stack
                    return 0;
                    break;

                case O_MUL:
                    MAKE_E;
                    GENERATE(I_MUL);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_SUB:
                    MAKE_E;
                    GENERATE(I_SUB);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_DIV:
                    MAKE_E;
                    GENERATE(I_DIV);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_LESS:
                    MAKE_E;
                    GENERATE(I_LESS);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_LESS_EQUAL:
                    MAKE_E;
                    GENERATE(I_LESSQ);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_MORE:
                    MAKE_E;
                    GENERATE(I_GREAT);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_MORE_EQUAL:
                    MAKE_E;
                    GENERATE(I_GREATQ);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_EQUAL:
                    MAKE_E;
                    GENERATE(I_EQ);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;

                case O_NOT_EQUAL:
                    MAKE_E;
                    GENERATE(I_NOTEQ);
                    CLEAN_STACK;
                    MOVE_TOP;
                    return 0;
                    break;
                default:
                    fprintf(stderr,"invalid operator in reduce function");
                    exit(ERR_INTERNAL);
            }//end of switch
        }

    return -2;


}

int expressions(symbol_variable ** last_p){

    t_pstack *item; //variable for stack
    expr_stack_init(&item); //call init stack ->malloc space for stack
    t_action action;        // variable for check action from symbol from precedence table
    expr_term tmp;          //temporary variable for checking rule in precedence table
    int error = 0;          //check errors

    bool end = false;       //if we reach and of expression translate return -1 end =true and we must stop with readin new tokens


    while (1) {
        if (end == false)
            if( get_next_token(token) == LEX_ERR)
                return LEX_ERR;

        // only ID INT DOUBLE INT STRING need informations about themselves
        if (token->state == S_ID || token->state == S_ID_FULL || token->state == S_DOUBLE || token->state == S_INT || token->state == S_STR){

            //malloc space for copy of token
            t_token * tmp_token = malloc(sizeof(struct Ttoken));
            if (tmp_token == NULL)
                return ERR_INTERNAL;

            //malloc space for string inside token +1 becose /0
            tmp_token->state=token->state;
            tmp_token->int_data=token->int_data;
            tmp_token->double_data=token->double_data;

            tmp_token->string_data.data=malloc(sizeof(char)*token->string_data.pos+1);
            if (tmp_token->string_data.data == NULL)
                return ERR_INTERNAL;
            strcpy(tmp_token->string_data.data,token->string_data.data);//memcpy

            //assign new token to ID
            item->item[item->last_term+2].token=tmp_token;
            //save poiners for free
            Stack_push(&Garbage,tmp_token);
            Stack_push(&Garbage,tmp_token->string_data.data);
            //printf("%d",item->item[item->last_term+2].token->state);
             }

        tmp = expr_translate(token->state, &error);

        if (error == -1) {
            end = true;
            tmp = O_EMPTY;
        }

        //check action in precedence table
        action = expr_get_rule(&item, tmp);


        switch (action) {
            case REDUCE:
                error = expr_reduce(&item);


                if(error==-2){
                    free(item);
                    return SYNTAX_ERR;
                }
                if (error == SEM_ERR ) {
                    free(item);
                    return SEM_ERR;
                }
                if(end == false)
                    push_token_back();
                break ;

            case SHIFT:
                error = expr_push_pos(&item, O_STOP, item->last_term + 1);
                if (error == -1) {
                    free(item);
                    return ERR_INTERNAL;
                }
                error = expr_push_top(&item, tmp);
                if (error == -1) {
                    free(item);
                    return ERR_INTERNAL;
                }
                break;

            case ERROR:
                free(item);
                return SYNTAX_ERR;
                break;

            case EQUAL:
                error = expr_push_top(&item, tmp);
                break;

            case END_EXPRESSION:
                push_token_back();
                (*last_p)=item->item[item->top-1].data;
                free(item);
                return OK;

            default:
                break;
        }
        if (error == -1){
            free(item);
            return ERR_INTERNAL;
        }
//        expr_print_stack(&item);
  //       printf("\n");

    }
}
