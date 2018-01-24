/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "interpret.h"
#include "stack.h"

Stack instr_next;
symbol_variable *RETURN;


int math(int c, symbol_variable *adr1, symbol_variable *adr2, symbol_variable *adr3)
{
    int ai = 0, bi = 0;
    double ad = 0, bd = 0;

    if (adr1->type == variable_string && adr2->type == variable_string && c == '+')
    {
        if (!adr1->init || !adr2->init)
            return NONINI_ERR;
        
        adr3->type = variable_string;
        symbol_variable_assign_string(adr3, adr1->value.value_string); // asssign string from adr1 to result
        symbol_variable_concat_string(adr3, adr2->value.value_string); //concat string from ad2 to result
        adr3->init = 1;
        return 0;
    }
    else if((adr1->type == variable_string || adr2->type == variable_string) && c == '+')
    {   //concat str + int, int + str, str + double, double + str
        if (!adr1->init || !adr2->init)
            return NONINI_ERR;
        
        adr3->type = variable_string;
        char *convert = NULL;
        
        if (adr1->type == variable_string && adr2->type == variable_double)
        {
            double transform_value = adr2->value.value_double;
            int length = snprintf( NULL, 0, "%g", transform_value);
            convert = malloc(length + 1);
            if (convert == NULL)
            {
                fprintf(stderr, "Error at allocation of converted string!\n");
                exit(ERR_INTERNAL);
            }
            snprintf(convert, length + 1, "%g", transform_value);
            symbol_variable_assign_string(adr3, adr1->value.value_string);
            symbol_variable_concat_string(adr3, convert);
            free(convert);
        }
        else
        { //double + string
            if (adr1->type == variable_double && adr2->type == variable_string)
            {
                double transform_value = adr1->value.value_double;
                int length = snprintf( NULL, 0, "%g", transform_value);
                convert = malloc(length + 1);
                if (convert == NULL)
                {
                    fprintf(stderr, "Error at allocation of converted string!\n");
                    exit(ERR_INTERNAL);
                }
                snprintf(convert, length + 1, "%g", transform_value);
                symbol_variable_assign_string(adr3, convert);
                symbol_variable_concat_string(adr3, adr2->value.value_string);
                free(convert);
            }
            else
            { //str + int
                if (adr1->type == variable_string && adr2->type == variable_integer)
                {
                    int transform_value = adr2->value.value_integer;
                    int length = snprintf( NULL, 0, "%d", transform_value);
                    convert = malloc(length + 1);
                    if (convert == NULL)
                    {
                        fprintf(stderr, "Error at allocation of converted string!\n");
                        exit(ERR_INTERNAL);
                    }
                    snprintf(convert, length + 1, "%d", transform_value);
                    symbol_variable_assign_string(adr3, adr1->value.value_string);
                    symbol_variable_concat_string(adr3, convert);
                    free(convert);
                }
                else
                { // int + str
                    if (adr1->type == variable_integer && adr2->type == variable_string)
                    {
                        int transform_value = adr1->value.value_integer;
                        int length = snprintf( NULL, 0, "%d", transform_value);
                        convert = malloc(length + 1);
                        if (convert == NULL)
                        {
                            fprintf(stderr, "Error at allocation of converted string!\n");
                            exit(ERR_INTERNAL);
                        }
                        snprintf(convert, length + 1, "%d", transform_value);
                        symbol_variable_assign_string(adr3, convert);
                        symbol_variable_concat_string(adr3, adr2->value.value_string);
                        free(convert);
                    }
                }
            }
        }
        
        adr3->init = 1;
        return 0;
    }
    else if (adr1->type == variable_string || adr2->type == variable_string)
        return TYPE_ERR;

    if (adr1->type == variable_double || adr2->type == variable_double)
        adr3->type = variable_double;
    else
        adr3->type = variable_integer;

    if (adr1->type == variable_double)
        ad = adr1->value.value_double;
    else if (adr1->type == variable_integer)
        ai = adr1->value.value_integer;
    else
        return TYPE_ERR;

    if (adr2->type == variable_double)
        bd = adr2->value.value_double;
    else if (adr2->type == variable_integer)
        bi = adr2->value.value_integer;
    else
        return TYPE_ERR;

    if (!adr1->init || !adr2->init)
        return NONINI_ERR;
    
    adr3->init = 1;

    switch (c)
    {
        case '+':
            if (adr3->type == variable_double)
            {
                //Adr1-double, Adr2-double
                if (adr1->type == variable_double && adr2->type == variable_double)
                {
                    adr3->value.value_double = ad + bd;
                }
                else
                {
                    //Adr1-double, Adr2-int
                    if (adr1->type == variable_double && adr2->type == variable_integer)
                    {
                        adr3->value.value_double = ad + bi;
                    }
                    else
                    {
                        //Adr1-int, Adr2-double
                        if (adr1->type == variable_integer && adr2->type == variable_double)
                        {
                            adr3->value.value_double = ai + bd;
                        }
                    }
                }
            }
            else
                //Adr1-int, Adr2-int
                if (adr1->type == variable_integer && adr2->type == variable_integer)
                {
                    adr3->value.value_integer = ai + bi;
                }
            //TODO String ?
            break;
            
        case '-':
            if (adr3->type == variable_double)
            {
                //Adr1-double, Adr2-double
                if (adr1->type == variable_double && adr2->type == variable_double)
                {
                    adr3->value.value_double = ad - bd;
                }
                else
                {
                    //Adr1-double, Adr2-int
                    if (adr1->type == variable_double && adr2->type == variable_integer)
                    {
                        adr3->value.value_double = ad - bi;
                    }
                    else
                    {
                        //Adr1-int, Adr2-double
                        if (adr1->type == variable_integer && adr2->type == variable_double)
                        {
                            adr3->value.value_double = ai - bd;
                        }
                    }
                }
            }
            else
                if (adr1->type == variable_integer && adr2->type == variable_integer)
                {
                    adr3->value.value_integer = ai - bi;
                }
            break;
            
        case '*':
            if (adr3->type == variable_double)
            {
                //Adr1-double, Adr2-double
                if (adr1->type == variable_double && adr2->type == variable_double)
                {
                    adr3->value.value_double = ad * bd;
                }
                else
                {
                    //Adr1-double, Adr2-int
                    if (adr1->type == variable_double && adr2->type == variable_integer)
                    {
                        adr3->value.value_double = ad * bi;
                    }
                    else
                    {
                        //Adr1-int, Adr2-double
                        if (adr1->type == variable_integer && adr2->type == variable_double)
                        {
                            adr3->value.value_double = ai * bd;
                        }
                    }
                }
            }
            else
                if (adr1->type == variable_integer && adr2->type == variable_integer)
                {
                    adr3->value.value_integer = ai * bi;
                }
            break;
            
        case '/':
            if (adr3->type == variable_double)
            {
                //Adr1-double, Adr2-double
                if (adr1->type == variable_double && adr2->type == variable_double)
                {
                    if (bd == 0.0) //Div by zero
                        return DIV_ZERO_ERR;
                    adr3->value.value_double = ad / bd;
                }
                else
                {
                    //Adr1-double, Adr2-int
                    if (adr1->type == variable_double && adr2->type == variable_integer)
                    {
                        if (bi == 0) //Div by zero
                            return DIV_ZERO_ERR;
                        adr3->value.value_double = ad / bi;
                    }
                    else
                    {
                        //Adr1-int, Adr2-double
                        if (adr1->type == variable_integer && adr2->type == variable_double)
                        {
                            if (bd == 0.0) //Div by zero
                                return DIV_ZERO_ERR;
                            adr3->value.value_double = ai / bd;
                        }
                    }
                }
            }
            else
            {
                if (adr1->type == variable_integer && adr2->type == variable_integer)
                {
                    if (bi == 0) //Div by zero
                        return DIV_ZERO_ERR;
                    adr3->type = variable_integer;
                    adr3->value.value_integer = ai / bi;
                }
            }
            break;
    }
    return 0;
}

int compare(char c, symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3)
{
    if (adr1->type == variable_string || adr2->type == variable_string)
        return TYPE_ERR;
    
    if (adr1->type == variable_bool || adr2->type == variable_bool)
        return TYPE_ERR;
    
    adr3->type = variable_bool;
    double a = 0, b = 0;
    if (!adr1->init || !adr2->init)
        return NONINI_ERR;

    switch (adr1->type)
    {
        case variable_integer:
            a = adr1->value.value_integer;
            break;
        case variable_double:
            a = adr1->value.value_double;
            break;
        case variable_bool: //Could be int becouse of 1 or 0 in C but for better read?
            a = adr1->value.value_bool;
            break;
        default:
            return TYPE_ERR;
    }

    switch (adr2->type)
    {
        case variable_integer:
            b = adr2->value.value_integer;
            break;
        case variable_double:
            b = adr2->value.value_double;
            break;
        case variable_bool: //Could be int becouse of 1 or 0 in C but for better read?
            b = adr2->value.value_bool;
            break;
        default:
            return TYPE_ERR;
    }

    switch (c)
    {
        case 'l':
            adr3->value.value_bool = a < b ? true : false;
            break;
        case '<':
            adr3->value.value_bool = a <= b ? true : false;
            break;
        case 'g':
            adr3->value.value_bool = a > b ? true : false;
            break;
        case '>':
            adr3->value.value_bool = a >= b ? true : false;
            break;
        case '=':
            adr3->value.value_bool = a == b ? true : false;
            break;
        case '!':
            adr3->value.value_bool = a != b ? true : false;
            break;
        default:
            return ERR_UNKNOWN;
    }
    adr3->init = 1;
    return 0;
}

int builtinfc(int i, symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3)
{
    switch (i)
    {
        case I_READ_INT:
            adr3->type = variable_integer;
            adr3->value.value_integer = readInt();
            if (adr3->value.value_integer < 0)
                return IO_ERR;
            break;
            
        case I_READ_D:
            adr3->type = variable_double;
            adr3->value.value_double = readDouble();
            if (adr3->value.value_double < 0.0)
                return IO_ERR;
            break;
            
        case I_READ_STR:
            adr3->type = variable_string;
            adr3->value.value_string = readString();
            break;
            
        case I_LENGTH:
            if (adr1->type != variable_string)
                return TYPE_ERR;
            if (adr1->init != 1)
                return NONINI_ERR; //There shoul be some string. Error in length should not happend.
            adr3->type = variable_integer;
            adr3->value.value_integer = length(adr1->value.value_string); //should not return -1 but check it
            if (adr3->value.value_integer < 0)
            {
                return ERR_UNKNOWN; //TEST THIS!!! MAYBE NOT RIGHT.
            }
            break;

        case I_COMPARE:
            if (adr1->type != variable_string || adr2->type != variable_string)
                return TYPE_ERR;
            if (!adr1->init || !adr2->init)
                return NONINI_ERR;
            adr3->type = variable_integer;
            adr3->value.value_integer = compare_string(adr1->value.value_string, adr2->value.value_string);
            break;
            
        case I_FIND:
            if (adr1->type != variable_string || adr2->type != variable_string)
                exit(TYPE_ERR);
            if (!adr1->init || !adr2->init)
                return NONINI_ERR;
            adr3->type = variable_integer;      
            adr3->value.value_integer = find(adr1->value.value_string, adr2->value.value_string);
            break;
            
        case I_SORT:
            if (adr1->type != variable_string)
                exit(TYPE_ERR);
            if (!adr1->init)
                return NONINI_ERR;
            adr3->type = variable_string;
            adr3->value.value_string = sort(adr1->value.value_string);
            break;
            
        default:
            return ERR_UNKNOWN;
    }
    adr3->init = 1;
    return 0;
}

int call_substr(symbol_variable (*adr1)[], symbol_variable *adr3)
{

    if ((*adr1)[0].type != variable_string)
        return TYPE_ERR;
    if ((*adr1)[1].type != variable_integer)
        return TYPE_ERR;
    if ((*adr1)[2].type != variable_integer)
        return TYPE_ERR;
    if (adr3->type == variable_void)
    {
        adr3->type = variable_string;
    }
    if (adr3->type != variable_string)
        return TYPE_ERR;
    if (!(*adr1)[0].init && !(*adr1)[1].init && !(*adr1)[2].init)
        return NONINI_ERR;
    adr3->value.value_string = substr((*adr1)[0].value.value_string, (*adr1)[1].value.value_integer, (*adr1)[2].value.value_integer);
    if (adr3->value.value_string == NULL)
    {
        return ERR_UNKNOWN;
    }
    adr3->init = 1;
    return 0;
}
//could use only on bool value -> result of compare.
int negate(symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3)
{
    if (adr1->type != variable_bool) //check this becouse od constants while(4)
    {
        return TYPE_ERR;
    }
    
    //adr3->type = variable_bool;
    if (adr1->value.value_bool == true)
    {
        adr3->value.value_bool = false;
    }
    else
    {
        adr3->value.value_bool = true;
    }
    adr3->init = 1;
    return 0;
}

int go_to(ins_list* list, symbol_variable* adr1, void *adr2, ins_item_ptr ptr)
{

    if (adr1 == NULL)
    {
        list->act = ptr;
    }
    else
    {
        if (adr1->init == 1)
        {
            if (adr1->value.value_bool == true) {
                list->act = ptr;
            }
        }
        else
            return NONINI_ERR;
    }
    return 0;
}

int assign_value(symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3, int cnt)
{
    if (adr3->type == variable_void)
    {
        adr3->type = adr1->type;
    }
    
    //pretypovani
    if (adr3->type == variable_double && adr1->type == variable_integer)
    {
        adr3->value.value_double = adr1->value.value_integer;
        adr3->init = 1;
        return 0;
    }

    if (adr3->type != adr1->type)
    {
        return TYPE_ERR;
    }
    
    if (adr1->init != 1)
    {
        if (cnt == 0) {
            return OTHER_SEM_ERR;
        }
        
        return NONINI_ERR; //to co prirazuji musi byt inicializovane
    }
    else
    {
        switch (adr3->type)
        {
            case variable_integer:
                adr3->value.value_integer = adr1->value.value_integer;
                break;
            
            case variable_double:
                adr3->value.value_double = adr1->value.value_double;
                break;
                
            case variable_bool:
                adr3->value.value_bool = adr1->value.value_bool;
                break;
                
            case variable_string:
                symbol_variable_assign_string(adr3, adr1->value.value_string);
                break;
                
            default:
                break;
        }
        adr3->init = 1;
    }
    return 0;
}

int ass_return(symbol_variable* adr1, symbol_variable* adr2, symbol_variable* adr3)
{
    adr1 = RETURN;
    
    if (adr3->type == variable_void)
    {
        adr3->type = adr1->type;
    }
    
    if (adr3->type != adr1->type)
    {
        return TYPE_ERR;
    }
    else
    {
        switch (adr3->type)
        {
            case variable_integer:
                adr3->value.value_integer = adr1->value.value_integer;
                break;
                
            case variable_double:
                adr3->value.value_double = adr1->value.value_double;
                break;
                
            case variable_bool:
                adr3->value.value_bool = adr1->value.value_bool;
                break;
                
            case variable_string:
                symbol_variable_assign_string(adr3, adr1->value.value_string);
                break;
                
            default:
                break;
        }
        adr3->init = 1;
    }
    return 0;
}


int interpret(ins_list *list)
{
    int counter_list = 0;
    Stack_init(&instr_next);
    int result = 0;
    first(list);
    
    while (list->act != NULL)
    {
        switch (list->act->ins.type)
        {
            case I_ADD:
                result = math('+', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_SUB:
                result = math('-', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_MUL:
                result = math('*', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_DIV:
                result = math('/', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_LESS:
                result = compare('l', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_LESSQ:
                result = compare('<', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_GREAT:
                result = compare('g', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_GREATQ:
                result = compare('>', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_EQ:
                result = compare('=', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_NOTEQ:
                result = compare('!', list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_NOT:
                result = negate(list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_GOTO:
                result = go_to(list, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_LABEL:;
                break;
                
            case I_READ_INT:
                result = builtinfc(I_READ_INT, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_READ_D:
                result = builtinfc(I_READ_D, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_READ_STR:
                result = builtinfc(I_READ_STR, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_PRINT:;
                symbol_variable *aux_print = list->act->ins.addr1;
                if (!aux_print->init)
                    result = NONINI_ERR;
                else
                    print(aux_print);
                break;
                
            case I_LENGTH:
                result = builtinfc(I_LENGTH, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;

            case I_SUBSTR:
                result = call_substr(list->act->ins.addr1, list->act->ins.addr3);
                break;
                
            case I_COMPARE:
                result = builtinfc(I_COMPARE, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_FIND:
                result = builtinfc(I_FIND, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_SORT:
                result = builtinfc(I_SORT, list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_ASSIGN:
                result = assign_value(list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3, counter_list);
                break;
                
            case I_ASS_RET:
                result = ass_return(list->act->ins.addr1, list->act->ins.addr2, list->act->ins.addr3);
                break;
                
            case I_CALL:
                counter_list = 1;
                Stack_push(&instr_next, list->act->next);
                symbol_function *aux = list->act->ins.addr1;
                make_mask(aux->local_table);
                assign_args(list->act->ins.addr1);
                if (aux->func_ins_list->first == NULL)
                {
                    fprintf(stderr, "Return instruction not found!\n");
                    result = NONINI_ERR;
                }
                list->act = aux->func_ins_list->first;
                continue;
                
            case I_RETURN:
                RETURN = list->act->ins.addr1;
                symbol_function *aux2 = list->act->ins.addr2;
                apply_mask(aux2);
                list->act = Stack_top(&instr_next);
                Stack_not_free_pop(&instr_next);
                continue;

            case I_HALT:
                return 0;
            default:
                return ERR_UNKNOWN;

        }

        
        if (list->act->next == NULL)
        {
            fprintf(stderr, "Return instruction not found!\n");
            result = NONINI_ERR;
        }
        
        if (result != 0)
        {
            return result;
        }
        succ(list);
    }
    return 0;
}

void dispose_instr_next()
{
    while(instr_next.first != NULL)
    {
        Stack_not_free_pop(&instr_next);
    }
}
