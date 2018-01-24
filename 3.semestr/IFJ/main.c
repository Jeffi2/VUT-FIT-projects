/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/


#include "scaner.h"
#include "parser.h"


//GLOBAL VARIABLE
FILE *file;
Stack *mask_stack; //needs to set dispose

int main(int argc, const char * argv[])
{
    
    //Argument check
    if (argc != 2)
    {
        fprintf(stderr, "Wrong parameters.\n");
        return ERR_INTERNAL;
    }
    
    //File open check
    if ((file = fopen(argv[1], "r")) == NULL)
    {
        fprintf(stderr, "Can't open file.\n");
        return ERR_INTERNAL;
    }
    //INIT STACK FOR POINTERS
    Stack_init(&Garbage);
    Stack_init(&const_table);
    
    Stack martin_stack;
    Stack_init(&martin_stack);
    mask_stack = &martin_stack;
    
    tSymbolTable Sym_table; //table of classes
    bst_init(&Sym_table);
    
    
    set_file(file); //Set file for lexical analysis
    //instruction list init
    Glob_INS_LIST = create_fnc_list();
    
    
    int result;
    
    //first pass in file
    First_look = 1;
    result = parse(&Sym_table);
    
    //if input file has Main.run and essential
    if(result == OK && !has_main_run() )
        result = SEM_ERR;
    
    if (result == OK)
    {
        fclose(file);
        
        if ((file = fopen(argv[1], "r")) == NULL)
        {
            fprintf(stderr, "Can't open file.\n");
            return ERR_INTERNAL;
        }

        set_file(file);
        
        First_look = 0;
        //second pass of syntax scanner
        result = parse(&Sym_table);

        symbol_function * fun = find_fun("Main.run");

        generate_instruction(Glob_INS_LIST, fun, NULL, NULL, I_CALL);
        generate_instruction(Glob_INS_LIST, NULL, NULL, NULL, I_HALT);

    }

    //debug
    //printf("Instrukcni paska GLOBAL: ");
    //print_ins_list_debug(Glob_INS_LIST);
    //printf("\n");
    //printf("##########################################################\n");

    //debug
    //printf("Parser end \n");
    //printf("##########################################################\n");


    //interpret call
    if(result == OK)
    {
        //printf("interpret start: \n");
        result = interpret(Glob_INS_LIST);
    }

    
/*
    switch (result)
    {
        case OK:
            printf("ok = 0\n");
            break;
            
        case LEX_ERR:
            printf("LEX_ERR = 1\n");
            break;
            
        case SYNTAX_ERR:
            printf("SYNTAX_ERR = 2\n");
            break;
            
        case SEM_ERR:
            printf("SEM_ERR = 3\n");
            break;

        case TYPE_ERR:
            printf("TYPE_ERR = 4\n");
            break;

        case IO_ERR:
            printf("IO_ERR = 7\n");
            break;
            
        case NONINI_ERR:
            printf("NON_INIT_VAR = 8\n");
            break;
            
        case ERR_INTERNAL:
            printf("INTERNAL_ERR = 99\n");
            result = - result;
            break;
            
        default:
            break;
    }
    
*/
    dispose_mask_stack();
    dispose_instr_next();
    dispose_list(Glob_INS_LIST);
    dispose_const_table();
    bst_dispose(&Sym_table);
    Stack_dispose(&Garbage);
    
    //close opened file
    fclose(file);
    
    return -result;       //My debug
    
}