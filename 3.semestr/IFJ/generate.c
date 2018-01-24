/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "generate.h"

void init_list (ins_list *i_list)
{
    i_list->act = NULL;
    i_list->first = NULL;
    i_list->last = NULL;
    return;
}

void dispose_list (ins_list *i_list)
{
    ins_item_ptr aux = i_list->first;
    if (aux != NULL)
    {
        while (aux != i_list->last)
        {
            aux = aux->next;
            free(aux->prev);
        }
        free(i_list->last);
    }
    i_list->act = NULL;
    free(i_list);
}

void insert_first (ins_list *i_list, void *ad1, void *ad2, void *ad3, int ins_val)
{
    ins_item_ptr new = NULL;
    if ((new = malloc(sizeof(struct ins_item))) == NULL)
    {
        fprintf(stderr, "Error at allocation!\n");
        exit(ERR_INTERNAL);
    }
    else
    {
        new->ins.addr1 = ad1;
        new->ins.addr2 = ad2;
        new->ins.addr3 = ad3;
        new->ins.type = ins_val;
        new->prev = NULL;
        new->next = i_list->first;
        if (i_list->first == NULL)
            i_list->first = i_list->last = new;
        else
        {
            i_list->first->prev = new;
            i_list->first = new;
        }
    }
}

void insert_last(ins_list *i_list, void *ad1, void *ad2, void *ad3, int ins_val)
{
    ins_item_ptr new = NULL;
    if ((new = malloc(sizeof(struct ins_item))) == NULL)
    {
        fprintf(stderr, "Error at allocation!\n");
        exit(ERR_INTERNAL);
    }
    else
    {
        new->ins.addr1 = ad1;
        new->ins.addr2 = ad2;
        new->ins.addr3 = ad3;
        new->ins.type = ins_val;
        new->next = NULL;
        new->prev = i_list->last;
        if (i_list->last == NULL)
            i_list->last = i_list->first = new;
        else
        {
            i_list->last->next = new;
            i_list->last = new;
        }
    }
}

void first (ins_list *i_list)
{
    i_list->act = i_list->first;
}

void last (ins_list *i_list)
{
    i_list->act = i_list->last;
}
void delete_first (ins_list *i_list)
{
    if (i_list->first != NULL)
    {
        ins_item_ptr aux = i_list->first;
        if (i_list->first == i_list->act)
            i_list->act = NULL;
        if (i_list->first == i_list->last)
        {
            i_list->first = NULL;
            i_list->last = NULL;
        }
        else
        {
            i_list->first = i_list->first->next;
            i_list->first->prev = NULL;
        }
        free(aux);
    }
}

void succ (ins_list *i_list)
{
    if (i_list->act != NULL)
    {
        if (i_list->act != i_list->last)
            i_list->act = i_list->act->next;
        else
            i_list->act = NULL;
    }
}

void pred (ins_list *i_list)
{
    if (i_list->act != NULL)
    {
        if (i_list->act != i_list->first)
            i_list->act = i_list->act->prev;
        else
            i_list->act = NULL;
    }
}

//TODO select in which list should instruction be generated -> function list..
void generate_instruction(ins_list *list, void *ad1, void *ad2, void *ad3, int ins_val)
{
    insert_last(list, ad1, ad2, ad3, ins_val);
}

//create label as a ins_intem field
ins_item_ptr make_label()
{
    ins_item_ptr new = malloc(sizeof(struct ins_item));
    if (new == NULL)
    {
        fprintf(stderr, "Error at allocation!\n");
        exit(ERR_INTERNAL);
    }
    new->ins.type = I_LABEL;
    new->next = NULL;
    new->prev = NULL;
    return new;
}

//insert label as a field into i_list
void insert_label(ins_item_ptr label_field, ins_list *i_list)
{
    label_field->prev = i_list->last;
    if (i_list->last == NULL)
        i_list->last = i_list->first = label_field;
    else
    {
        i_list->last->next = label_field;
        i_list->last = label_field;
    }
}

ins_list* create_fnc_list()
{
    ins_list *new = malloc(sizeof(ins_list));
    if (new == NULL)
    {
        fprintf(stderr, "Error at allocation!\n");
        exit(ERR_INTERNAL);
    }
    init_list(new);
    return new;
}

void print_ins_list_debug(ins_list *list)
{
    first(list);
    while (list->act != NULL)
    {
        switch (list->act->ins.type){
            case I_ADD: printf("%c ", '+');
                break;
            case I_SUB: printf("%c ", '-');
                break;
            case I_MUL: printf("%c ",'*');
                break;
            case I_DIV: printf("%c ",'/');
                break;
            case I_PRINT: printf("%s ","Print");
                break;
            case I_ASSIGN: printf("%s ","Assign");
                break;
            case I_NOT: printf("%s ","Not");
                break;
            case I_LESS: printf("%s ","<");
                break;
            case I_GREAT: printf("%s ",">");
                break;
            case I_LESSQ: printf("%s ","<=");
                break;
            case I_GREATQ: printf("%s ",">=");
                break;
            case I_EQ: printf("%s ","==");
                break;
            case I_NOTEQ: printf("%s ","!=");
                break;
            case I_LABEL: printf("%s ","Label");
                break;
            case I_CALL: printf("%s ","Call");
                break;
            case I_RETURN: printf("%s ","Return");
                break;
            case I_GOTO: printf("%s ","Goto");
                break;
            case I_READ_INT: printf("%s ","Read_int");
                break;
            case I_READ_D: printf("%s ","Read_double");
                break;
            case I_READ_STR: printf("%s ","Read_string");
                break;
            case I_LENGTH: printf("%s ","Length");
                break;
            case I_SUBSTR: printf("%s ","Substr");
                break;
            case I_COMPARE: printf("%s ","Compare");
                break;
            case I_FIND: printf("%s ","Find");
                break;
            case I_SORT: printf("%s ","Sort");
                break;
            case I_HALT: printf("%s ","HALT!");
                break;
                
            default:printf("%d ",list->act->ins.type);
                
        }
        succ(list);
    }
}
