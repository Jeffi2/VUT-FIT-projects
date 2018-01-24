/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/


#include "lltable.h"

#define FALSE 0
#define TRUE 1

int First_look;

void print_error(const char * str)
{
    fprintf(stderr,"Error in function: %s, with this token: %d\n", str, token->state);
    if(token->state == S_ID || token->state == S_ID_FULL)
        fprintf(stderr,"Error: %s !!\n", token->string_data.data);
}

int first_look_loop()
{
    int i = 0;

    if(token->state == S_LCUR)
        i++;

    if(token->state == S_RCUR)
        i--;

    if(token->state == S_EOF)
        return SYNTAX_ERR;

    while (i>=0)
    {
        if (get_next_token(token) == LEX_ERR) return LEX_ERR; // Error lexical analyzer

        if(token->state == S_LCUR)
            i++;

        if(token->state == S_RCUR)
            i--;

        if(token->state == S_EOF)
            return SYNTAX_ERR;
    }

    return OK;
}


int prog()
{
    int result;

    switch (token->state)
    {
        //1: <prog> -> class id.simple { <inclass> } <prog>
        case S_CLASS:
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR; // Error lexical analyzer
            if (token->state != S_ID) return SYNTAX_ERR; //next expected token is id.simple

            //first look insert name of tha class
            if (First_look)
                result = new_class(token->string_data.data);
            else
                result = set_class(token->string_data.data);

            if (result != OK) return result;


            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR; // Error lexical analyzer
            if (token->state != S_LCUR) return SYNTAX_ERR; //next expected token is {


            //pop this token and read next one
            if (get_next_token(token) == LEX_ERR) return LEX_ERR; // Error lexical analyzer

            result = inclass();   // go  to <inclass>
            if (result != OK) return result;

            if (token->state != S_RCUR) return SYNTAX_ERR; //next expected token is }

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR; // Error lexical analyzer

            result = prog();//call the non terminal
            if (result != OK) return result;

            return OK;

        // 2: <prog> -> e
        case S_EOF:
            return OK;

        //if others token => ERROR
        default:
            print_error("prog");
            return SYNTAX_ERR;
    }
}

int inclass()
{
    int result;

    switch (token->state)
    {
        //3: <inclass> -> static <decl> <inclass>
        case S_STATIC:
            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = decl();    //go into <decl>
            if (result != OK) return result;

            result = inclass(); //go into <inclass>
            if (result != OK) return result;

            return OK;

        //4: <inclass> -> e
        case S_RCUR:
            return OK;

        default:
            print_error("inclass");
            return SYNTAX_ERR;
    }
}

int decl()
{
    int result;
    tType_id item;

    switch (token->state)
    {
        //5: <decl> -> <type> id.simple <varfunc>
        case S_INT_KEY:
        case S_DOUBLE_KEY:
        case S_STRING:

            item.id_type = token->state;     //Store a type of var_func

            result = type();    //go into <type>
            if (result != OK) return result;

            if (token->state != S_ID) return SYNTAX_ERR;    //expected id.simple


            copy_name(&item.name, token->string_data.data);
            Stack_push(&Garbage, item.name);

            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = varfunc(item); //go into <varfunc>
            if (result != OK) return result;

            return OK;

        //4: <decl> -> void id.simple <func>
        case S_VOID:
            item.id_type = S_VOID;

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            if (token->state != S_ID) return SYNTAX_ERR;    //expected id.simple

            copy_name(&item.name, token->string_data.data);
            Stack_push(&Garbage, item.name);

            //id we have processed so go on.... NEXT TOKEN
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = func(item); //go into <func>
            if (result != OK) return result;

            return OK;

        default:
            print_error("decl");
            return SYNTAX_ERR;
    }
}

int varfunc(tType_id item)
{
    int result;

    switch (token->state)
    {
        //7: <varfunc> 	-> = <expr> ;
        case S_ASSIGN:
            //we know that it is var, because assignment

            //save to table of symbol in first look
            if(First_look)
            {
                result = new_glob_variable(item);
                if(result != OK) return result;
            }


            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //find a var where is in the left side
            symbol_variable * var = find_var(item.name);
            if(var == NULL)
            {
                print_error("ERror in find static var ");
                exit(ERR_INTERNAL);
            }

            result = expr(var);    //go into <expr>
            if (result != OK) return result;

            //expected ;
            if (token->state != S_SEMICOLON) return SYNTAX_ERR;

            //so pop -> next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        //8: <varfunc> -> ;
        case S_SEMICOLON:
            //we are sure that variable

            //save to table of symbol
            if(First_look)
            {
                result = new_glob_variable(item);
                if(result != OK) return result;
            }

            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        //9: <varfunc> 	-> <func>
        case S_LTHE:
            //we are sure that it is function

            result = func(item);    //go into <func>
            if (result != OK) return result;

            return OK;

        default:
            print_error("varfunc");
            return SYNTAX_ERR;
    }
}


int func(tType_id func)
{
    int result;

    switch (token->state)
    {
        //10: <func> -> ( <parlist> ) { <infunc> }
        case S_LTHE:
            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            // insert new function and set global environment
            if (First_look)
                result = new_function(func);
            else
                result = set_function(func);

            if(result != OK) return result;

            result = parlist();    //go into <parlist>
            if (result != OK) return result;

            //after parlist we expected )
            if (token->state != S_RTHE) return SYNTAX_ERR;
            //expectation satisfied so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //now we expected {
            if (token->state != S_LCUR) return SYNTAX_ERR;
            //expectation satisfied so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;


            symbol_function * fun = Parser_position.fun_ptr->ptr.function;
            if(fun == NULL) //debug
            {
                print_error("Internall err. Missing a add func. In func.");
                exit(ERR_INTERNAL);
            }

            //change instruction list to function instruction list
            INS_LIST = fun->func_ins_list;


            if (First_look)
                result = first_look_loop();
            else
                result = infunc(); //go into <infunc>

            if (result != OK) return result;

            //expected }
            if (token->state != S_RCUR) return SYNTAX_ERR;
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //if the function is void so add return
            if(fun->return_type == variable_void && !First_look)
            {
                generate_instruction(INS_LIST, NULL, fun, NULL, I_RETURN);
            }

            /*
            if(!First_look)
            {
                //debug
                printf("Insrtukcni paska funkce '%s' : ", fun->name);
                print_ins_list_debug(INS_LIST);
                printf("\n");
                printf("##########################################################\n");
            }
            */

            //unset_function
            unset_function();

            //set global instruction list
            INS_LIST = Glob_INS_LIST;

            return OK;

        default:
            print_error("func");
            return SYNTAX_ERR;
    }
}

int parlist()
{
    int result;
    tType_id param;

    switch (token->state)
    {
        //11: <parlist> -> <type> id.simple <paritem>
        case S_INT_KEY:
        case S_DOUBLE_KEY:
        case S_STRING:

            //Store a type of param = local var
            param.id_type = token->state;

            result = type();    //go into <type>
            if (result != OK) return result;

            //expected id.simple
            if (token->state != S_ID) return SYNTAX_ERR;

            param.name = token->string_data.data;

            //save param to local table
            if(First_look)
            {
                result = add_arg(param);
                if(result != OK) return result;
            }

            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = paritem(); //go into <paritem>
            if (result != OK) return result;

            return OK;

        // 12: <parlist> -> e
        case S_RTHE:
            return OK;


        default:
            print_error("parlist");
            return SYNTAX_ERR;
    }
}

int paritem()
{
    int result;
    tType_id param;

    switch (token->state)
    {
        //13: <paritem> -> , <type> id.simple <paritem>
        case S_COMMA:
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //Store a type of param = local var
            param.id_type = token->state;

            result = type();    //go into <type>
            if (result != OK) return result;

            //expected id.simple
            if (token->state != S_ID) return SYNTAX_ERR;

            param.name = token->string_data.data;

            //save param to local tabel
            if(First_look)
            {
                result = add_arg(param);
                if(result != OK) return result;
            }

            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = paritem(); //go into <paritem>
            if (result != OK) return result;

            return OK;

            // 14: <paritem> -> e
        case S_RTHE:
            return OK;


        default:
            print_error("parlist");
            return SYNTAX_ERR;
    }
}

int infunc()
{
    int result;
    tType_id local_var;

    switch (token->state)
    {
        //15: <infunc> -> <type> id.simple <locvar> <infunc>
        //local variable declare
        case S_INT_KEY:
        case S_DOUBLE_KEY:
        case S_STRING:

            //Store a type of param = local var
            local_var.id_type = token->state;

            result = type();    //go into <type>
            if (result != OK) return result;

            //expected id.simple
            if (token->state != S_ID) return SYNTAX_ERR;


            copy_name(&local_var.name, token->string_data.data);
            Stack_push(&Garbage, local_var.name);

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = locvar(local_var); //go into <locvar>
            if (result != OK) return result;


            result = infunc(); //go into <infunc>
            if (result != OK) return result;
            return OK;

        //16: <infunc> 	-> <stat> <infunc>
        case S_ID:
        case S_ID_FULL:
        case S_LCUR:
        case S_IF:
        case S_RETURN:
        case S_WHILE:

            result = stat();    //go into <stat>
            if (result != OK) return result;

            result = infunc(); //go into <infunc>
            if (result != OK) return result;

            return OK;

        //17: <infunc> 	-> e
        case S_RCUR:
            return OK;

        default:
            print_error("infunc");
            return SYNTAX_ERR;
    }
}

int locvar(tType_id local_var)
{
    int result;

    switch (token->state)
    {
        //18: <locvar> 	-> ;
        case S_SEMICOLON:

            result = new_loc_variable(local_var);
            if (result != OK) return result;

            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        //19: <locvar> 	-> = <expr>;
        //19: <locvar>  -> = <assign>;
        case S_ASSIGN:

            result = new_loc_variable(local_var);
            if (result != OK) return result;

            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //find a local var
            symbol_variable * var = find_var(local_var.name);
            if(var == NULL)
            {
                print_error("error in locvar ... we are sure, that this var is exist");
                exit(ERR_INTERNAL);
            }

            //change because int cmp = ifj16.compare(...);
            result = assign(var);    //go into <expr>
            if (result != OK) return result;

            //expected ;
            if (token->state != S_SEMICOLON) return SYNTAX_ERR;

            //so pop -> next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        default:
            print_error("locvar");
            return SYNTAX_ERR;
    }
}

int stat()
{
    int result;
    tType_id var;
    symbol_variable * condition;

    switch (token->state)
    {
        //20: <stat> -> id.simple <assfunc> ;
        case S_ID:
            //can be fun or var
            var.id_type = S_ID;
            copy_name(&var.name, token->string_data.data);
            Stack_push(&Garbage, var.name);

            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = assfunc(var); // go into <assfunc>
            if (result != OK) return result;

            //expected semicolon
            if (token->state != S_SEMICOLON) return SYNTAX_ERR;
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        // 21: <stat> -> id.full <assfunc> ;
        case S_ID_FULL:
            //can be fun or var
            var.id_type = S_ID_FULL;
            copy_name(&var.name, token->string_data.data);
            Stack_push(&Garbage, var.name);

            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = assfunc(var); // go into <assfunc>
            if (result != OK) return result;

            //expected semicolon
            if (token->state != S_SEMICOLON) return SYNTAX_ERR;
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        // 22: <stat> -> if ( <expr> ) <stat> <is_else>
        case S_IF:
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            //expected (
            if (token->state != S_LTHE) return SYNTAX_ERR;

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //make label
            ins_item_ptr if_else = make_label();
            ins_item_ptr if_end = make_label();


            condition = make_help_var();

            //go into <expr>
            result = expr(condition);
            if (result != OK)
            {
                Stack_push(&Garbage, if_else);
                Stack_push(&Garbage, if_end);
                return result;
            }

            //expected )
            if (token->state != S_RTHE)
            {
                Stack_push(&Garbage, if_else);
                Stack_push(&Garbage, if_end);
                return SYNTAX_ERR;
            }


            //neg condition
            generate_instruction(INS_LIST, condition, NULL, condition, I_NOT);

            //condition jump
            generate_instruction(INS_LIST, condition, NULL, if_else, I_GOTO);
            //next token
            if (get_next_token(token) == LEX_ERR)
            {
                Stack_push(&Garbage, if_else);
                Stack_push(&Garbage, if_end);
                return LEX_ERR;
            }

            //go into <stat>
            result = stat();
            if (result != OK)
            {
                Stack_push(&Garbage, if_else);
                Stack_push(&Garbage, if_end);
                return result;
            }

            //go to end of if
            generate_instruction(INS_LIST, NULL, NULL,if_end, I_GOTO);

            result = is_else(if_else, if_end);
            if(result != OK) return result;

            return OK;

        //23: <stat> -> while ( <expr> ) <stat>
        case S_WHILE:
            //while is covered so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            //expected (
            if (token->state != S_LTHE) return SYNTAX_ERR;

            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            condition = make_help_var();

            //make label
            ins_item_ptr wh_begind = make_label();
            ins_item_ptr wh_end = make_label();

            //start of while
            insert_label(wh_begind, INS_LIST);

            //go into <expr>
            result = expr(condition);
            if (result != OK)
            {
                Stack_push(&Garbage, wh_end);
                return result;
            }

            //expected )
            if (token->state != S_RTHE)
            {
                Stack_push(&Garbage, wh_end);
                return SYNTAX_ERR;
            }

            //neg condition
            generate_instruction(INS_LIST, condition, NULL, condition, I_NOT);

            //test condition
            generate_instruction(INS_LIST, condition, NULL, wh_end, I_GOTO);

            //next token
            if (get_next_token(token) == LEX_ERR)
            {
                Stack_push(&Garbage, wh_end);
                return LEX_ERR;
            }

            //go into <stat>
            result = stat();
            if (result != OK)
            {
                Stack_push(&Garbage, wh_end);
                return result;
            }


            generate_instruction(INS_LIST, NULL, NULL, wh_begind, I_GOTO);

            insert_label(wh_end, INS_LIST);

            return OK;

        //24: <stat> -> <compstat>
        case S_LCUR:
            //go into <compstat>
            result = compstat();
            if (result != OK) return result;

            return OK;

        //25: <stat> -> return <expr_non> ;
        case S_RETURN:
            //return is covered, so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //go into <expr_non>
            result = expr_non();
            if (result != OK) return result;

            //expected ;
            if (token->state != S_SEMICOLON) return SYNTAX_ERR;
            //new token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        default:
            print_error("stat");
            return SYNTAX_ERR;
    }
}

int is_else(ins_item_ptr if_else, ins_item_ptr if_end)
{
    int result;

    switch (token->state)
    {
        case S_ELSE:
            //else branch
            insert_label(if_else, INS_LIST);

            //next token
            if (get_next_token(token) == LEX_ERR)
            {
                Stack_push(&Garbage, if_end);
                return LEX_ERR;
            }

            //go into <stat>
            result = stat();
            if (result != OK)
            {
                Stack_push(&Garbage, if_end);
                return result;
            }

            //end of if
            insert_label(if_end, INS_LIST);

            return OK;

        default:
            insert_label(if_else, INS_LIST);
            insert_label(if_end, INS_LIST);

            return OK;
    }
}


int assfunc(tType_id item)
{

    int result;

    //variable, for assign from function
    symbol_function * fun;

    switch (token->state)
    {
        //26: <assfunc> -> = <assign>
        case S_ASSIGN:
            //pop and next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            //we are sure that item must be var -> so check
            symbol_variable * var = find_var(item.name);
            if(var == NULL)
            {
                print_error("Not found var in left side in assign");
                return SEM_ERR;
            }


            result = assign(var);    //go into <assign>
            if (result != OK) return result;

            return OK;

            //27: <assfunc> -> ( <params> )
        case S_LTHE:
            //non assign call function
            //pop and next token

            fun  = find_fun(item.name);
            if (fun == NULL)
                return SEM_ERR;

            result = func_call(fun, NULL);
            if (result != OK) return result;

            return OK;

        default:
            print_error("assfunc");
            return SYNTAX_ERR;
    }
}




int compstat()
{
    int result;

    switch (token->state)
    {
        //36: <compstat> 	-> { <incompstat> }
        case S_LCUR:
            //LCUR is cover, so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            result = incompstat();    //go into <incompstat>
            if (result != OK) return result;

            //expected }
            if (token->state != S_RCUR) return SYNTAX_ERR;
            //next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;

            return OK;

        default:
            print_error("compstat");
            return SYNTAX_ERR;
    }
}



int incompstat()
{
    int result;

    switch (token->state)
    {
        //37: <incompstat> -> <stat> <incompstat>
        case S_ID:
        case S_ID_FULL:
        case S_LCUR:
        case S_IF:
        case S_RETURN:
        case S_WHILE:

            result = stat();    //go into <stat>
            if (result != OK) return result;

            result = incompstat();      //go into <incompstat>
            if (result != OK) return result;

            return OK;

        //38: <incompstat> -> e
        case S_RCUR:
            return OK;

        default:
            print_error("incompstat");
            return SYNTAX_ERR;
    }
}


int type()
{
    switch (token->state)
    {
        //39: <type> -> int
        case S_INT_KEY:
            //so next token
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        //40: <type> -> double
        case S_DOUBLE_KEY:
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        //41: <type> -> String
        case S_STRING:
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            return OK;

        default:
            print_error("type");
            return SYNTAX_ERR;
    }
}



int assign(symbol_variable * var)
{
    int result;
    symbol_function * function;


    switch (token->state)
    {
        //check that is function
        case S_ID:
        case S_ID_FULL:

            function = find_fun(token->string_data.data);

            if(function != NULL) //func exist
            {

                if(function->return_type == variable_void)
                    return NONINI_ERR;

                //next token
                if (get_next_token(token) == LEX_ERR) return LEX_ERR;

                symbol_variable * help_var = make_help_var();
                help_var->type = var->type;

                result = func_call(function, help_var);
                if(result != OK) return result;

                //assign return val
                generate_instruction(INS_LIST, help_var, NULL, var, I_ASSIGN);

                return OK;
            }
            else //it is expr;
            {
                //is not function, so go into  <expr>

                //id is not fun, so check is it var?
                symbol_variable * maybe_var = find_var(token->string_data.data);
                if(maybe_var == NULL)
                    return SEM_ERR;

                result = expr(var);
                if(result != OK) return result;

                return OK;
            }


        default:
            //go into expr
            result = expr(var);
            if(result != OK) return result;

            return OK;
    }

}

int expr_non()
{
    int result;
    symbol_function * function =  find_fun(Parser_position.fun);

    if(function->return_type == variable_void)
    {
        //function is void -> return without expr;
        //printf("fun: %s\n", Parser_position.fun);

        //generate return in void

        int expr_token = token->state;
        push_token_back();

        //call expression
        symbol_variable * expr_val = NULL;
        result = expressions(&expr_val);
        if (result != OK) return result;

        if (get_next_token(token) == LEX_ERR) return LEX_ERR;
        //printf("After expr, token: %d\n", token->state); //debug

        if (expr_token != token->state)
        {
            print_error("some expr in void fun");
            return TYPE_ERR;
        }
        
        generate_instruction(INS_LIST, NULL, function, NULL, I_RETURN);

        return OK;
    }
    else
    {
        //non void function

        symbol_variable * var = make_help_var();

        int result = expr(var);
        if(result != OK) return result;

        //generate return with value
        generate_instruction(INS_LIST, var, function, NULL, I_RETURN);

        return OK;
    }
}

int expr(symbol_variable * var)
{
    int result;

    //first look
    if (First_look)
    {
        if(token->state != S_SEMICOLON)
        {
            if (get_next_token(token) == LEX_ERR) return LEX_ERR;
            result = expr(NULL);
            if(result != OK) return result;
        }
        return OK;
    }
    else
    {
        //debug
        if(var == NULL)
        {
            print_error("Error: param is NULL in expr");
            exit(ERR_INTERNAL);
        }

        //printf("before expr, token: %d\n", token->state);   //debug
        int expr_token = token->state;
        push_token_back();

        //call expression
        symbol_variable * expr_val = NULL;
        result = expressions(&expr_val);
        if (result != OK) return result;

        if (get_next_token(token) == LEX_ERR) return LEX_ERR;
        //printf("After expr, token: %d\n", token->state); //debug

        if (expr_token == token->state)
        {
            print_error("no expression");
            return SYNTAX_ERR;
        }

        if (expr_val == NULL)
        {
            print_error("expr_errr --- NULL pointer to return !");
            exit(ERR_INTERNAL);
        }

        generate_instruction(INS_LIST, expr_val, NULL, var, I_ASSIGN);



        return OK;
    }

}
