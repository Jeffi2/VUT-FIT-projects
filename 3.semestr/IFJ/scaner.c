/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "scaner.h"
extern Stack Garbage;
extern FILE *file;

static FILE *source;
bool token_back = false;

void push_token_back()
{
    if (token_back == true)
    {
        //This should never happend!
        fprintf(stderr, "ERROR in push_token_back. I can remember only 1 token!\n");
    }
    else
    {
        token_back = true;
    }
}

void set_file(FILE *file)
{
    source = file;
}

int get_next_token(t_token *token)
{
    if (token_back == true)
    {
        token_back = false;
        return 0;
    }
    
    clean_token(token);
    
    token->state = S_START;
    int c; // Char from file
    char escape[5];

    while (1)
    {
        c = getc(source);
        switch (token->state)
        {
            case S_START:
                if (isspace(c) || c == '\n')
                    token->state = S_START;
                else if (isalpha(c) || c == '_' || c == '$')
                {
                    //First char starts ID
                    StrPutCharToString((char)c, &token->string_data);
                    token->state = S_ID;
                }
                else if (isdigit(c))
                {
                    //First number
                    StrPutCharToString((char)c, &token->string_data);
                    token->state = S_INT;
                }
                else if (c == EOF)
                {
                    //End of file
                    token->state = S_EOF;
                    return 0;
                }
                else if (c == '"')
                {
                    //Start of string
                    token->state = S_STR;
                }
                else if (c == '/')
                {
                    token->state = S_DIV;
                }
                else if (c == '+')
                {
                    token->state = S_ADD;
                    return 0;
                }
                else if (c == '-')
                {
                    token->state = S_SUB;
                    return 0;
                }
                else if (c == ';')
                {
                    token->state = S_SEMICOLON;
                    return 0;
                }
                else if (c == '(')
                {
                    token->state = S_LTHE;
                    return 0;
                }
                else if (c == ')')
                {
                    token->state = S_RTHE;
                    return 0;
                }
                else if (c == '{')
                {
                    token->state = S_LCUR;
                    return 0;
                }
                else if (c == '}')
                {
                    token->state = S_RCUR;
                    return 0;
                }
                else if (c == ',')
                {
                    token->state = S_COMMA;
                    return 0;
                }
                else if (c == '=')
                {
                    token->state = S_ASSIGN;
                }
                else if (c == '<')
                {
                    token->state = S_LESS;
                }
                else if (c == '*')
                {
                    token->state = S_MUL;
                }
                else if (c == '>')
                {
                    token->state = S_MORE;
                }
                else if (c == '!')
                {
                    token->state = S_NOT;
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_ID:
                if (isalnum(c) || c == '_' || c == '$')
                {
                    //token->state = S_ID; //Not needed state was set in start
                    StrPutCharToString((char)c, &token->string_data);
                }
                else if(c == '.')
                {
                    token->state = S_ID_CH;
                    StrPutCharToString((char)c, &token->string_data);
                }
                else
                {
                    //Final state
                    push_char_back(c);
                
                    StrPutCharToString('\0', &token->string_data);
                    keywords_check(token);

                    return 0;
                }
                break;
                
            case S_ID_CH:
                if (isalpha(c) || c == '_' || c == '$')
                {
                    StrPutCharToString((char)c, &token->string_data);
                    token->state = S_ID_FULL;
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_ID_FULL:
                if (isalnum(c) || c == '_' || c == '$')
                {
                    StrPutCharToString((char)c, &token->string_data);
                }
                else
                {
                    //Final state
                    push_char_back(c);
                    
                    StrPutCharToString('\0', &token->string_data);
                    return 0;
                }
                break;
                
            case S_INT:
                if (isdigit(c))
                {
                    StrPutCharToString((char)c, &token->string_data);
                }
                else if (c == '.')
                {
                    StrPutCharToString((char)c, &token->string_data);
                    token->state = S_DOUBLE_CH;
                }
                else if (c == 'e' || c == 'E')
                {
                    token->state = S_EXP_CH;
                    StrPutCharToString((char)c, &token->string_data);
                }

                else
                {
                    //Final state
                    push_char_back(c);
                    //Set end of string and convert to int
                    StrPutCharToString('\0', &token->string_data);
                    str_to_int(token);
                    
                    return 0;

                }
                break;
                
            case S_DOUBLE_CH:
                if (isdigit(c))
                {
                    StrPutCharToString((char)c, &token->string_data);
                    token->state = S_DOUBLE;
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_DOUBLE:
                if (isdigit(c))
                {
                    StrPutCharToString((char)c, &token->string_data);
                }
                else if (c == 'e' || c == 'E')
                {
                    token->state = S_EXP_CH;
                    StrPutCharToString((char)c, &token->string_data);
                }
                else
                {
                    //Final state
                    push_char_back(c);
                    //End string and set double value
                    StrPutCharToString('\0', &token->string_data);
                    str_to_double(token);
                    return 0;
                }
                break;
                
            case S_EXP_CH:

                if (c == '+' || c == '-')
                {
                    token->state = S_EXP_CH2;
                    StrPutCharToString((char)c, &token->string_data);
                }
                else if (isdigit(c))
                {
                    token->state = S_EXP_CH3;
                    StrPutCharToString((char)c, &token->string_data);
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_EXP_CH2:
                if (isdigit(c))
                {
                    token->state = S_EXP_CH3;
                    StrPutCharToString((char)c, &token->string_data);
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_EXP_CH3:
                if (isdigit(c))
                {
                    StrPutCharToString((char)c, &token->string_data);
                }
                else
                {
                    //Final state
                    push_char_back(c);
                    //End string and set double value
                    StrPutCharToString('\0', &token->string_data);
                    str_to_double(token);
                    token->state = S_DOUBLE;
                    
                    return 0;
                }
                break;
                
            case S_STR:
                if (c == '\\')
                {
                    token->state = S_ESC_CH;
                }
                else if (c < 31 || c == '\n' || c == EOF)
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                else if (c == '"')
                {
                    //Final state
                    //This state is empty string ""
                    StrPutCharToString('\0', &token->string_data);
                    return 0;

                }
                else
                {
                    token->state = S_STR;
                    StrPutCharToString((char)c, &token->string_data);
                }
                break;
                
            case S_ESC_CH:
                if (c == 't')
                {
                    token->state = S_STR;
                    StrPutCharToString('\t', &token->string_data);
                }
                else if (c == 'n')
                {
                    token->state = S_STR;
                    StrPutCharToString('\n', &token->string_data);
                }
                else if (c == '\\')
                {
                    token->state = S_STR;
                    StrPutCharToString('\\', &token->string_data);
                }
                else if (c == '"')
                {
                    token->state = S_STR;
                    StrPutCharToString('\"', &token->string_data);
                }
                else if (c >= '0' && c <= '3')
                {
                    token->state = S_ESC_CH1;
                    escape[0] = (char)c;
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_ESC_CH1:
                if (c >= '0' && c <= '7')
                {
                    token->state = S_ESC_CH2;
                    escape[1] = (char)c;
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_ESC_CH2:
                if (c >= '0' && c <= '7')
                {
                    escape[2] = (char)c;
                    escape[3] = '\0';
                    char *ptr1;
                    int octa1 = strtol(escape, &ptr1, 8);
                    if (octa1 == 0)
                    {
                        //esc \000 is not allowed
                        token->state = S_ERROR;
                        return LEX_ERR;
                    }
                    StrPutCharToString((char)octa1, &token->string_data);
                    token->state = S_STR;
                }
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            case S_DIV:
                if (c == '/')
                {
                    token->state = S_COM_LINE;
                }
                else if (c == '*')
                {
                    token->state = S_COM_BLOCK1;
                }
                else
                {
                    push_char_back(c);
                    return 0; //return div
                }
                break;
                
            case S_COM_LINE:
                if (c == '\n' || c == EOF)
                {
                    token->state = S_START;
                }
                break;
                
            case S_COM_BLOCK1:
                if (c == EOF)
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                else if (c == '*')
                {
                    token->state = S_COM_BLOCK2;
                }
                break;
                
            case S_COM_BLOCK2:
                if (c == EOF)
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                else if (c == '/')
                {
                    token->state = S_START;
                    //clean token
                }
                else if (c == '*')
                {
                    token->state = S_COM_BLOCK2;
                }
                else
                {
                    token->state = S_COM_BLOCK1;
                }
                break;
                
            case S_MUL:
            case S_ADD:
            case S_SUB:
            case S_SEMICOLON:
            case S_LTHE:
            case S_RTHE:
            case S_LCUR:
            case S_RCUR:
            case S_COMMA:
            case S_NOT_EQUAL:
                push_char_back(c);
                return 0;
                break;
                
            case S_ASSIGN:
                if (c == '=')
                {
                    token->state = S_EQUAL;
                }
                else
                {
                    push_char_back(c);
                    return 0;
                }
                break;
                
            case S_EQUAL:
                push_char_back(c);
                return 0;
                break;
                
            case S_LESS:
                if (c == '=')
                {
                    token->state = S_LESS_EQUAL;
                }
                else
                {
                    push_char_back(c);
                    return 0;
                }
                break;
                
            case S_LESS_EQUAL:
                push_char_back(c);
                return 0;
                break;
                
            case S_MORE:
                if (c == '=')
                    token->state = S_MORE_EQUAL;
                else
                {
                    push_char_back(c);
                    return 0;
                }
                break;
                
            case S_MORE_EQUAL: 
                push_char_back(c);
                return 0;
                break;
                
            case S_NOT:
                if (c == '=')
                    token->state = S_NOT_EQUAL;
                else
                {
                    token->state = S_ERROR;
                    return LEX_ERR;
                }
                break;
                
            default:
                break;
        }
    }
}

void clean_token (t_token* token)
{
    token->double_data = 0.0;
    token->int_data = 0;
    token->state = 0;
    token->string_data.data[0] = '\0';
    token->string_data.pos = 0;
}

t_token* token_init() {
    //alocate new token
    t_token* new_token = (t_token*) malloc(sizeof(t_token));
    if (new_token == NULL) {
        Stack_dispose(&Garbage);
        fprintf(stderr, "Error at allocation!\n");
        fclose(file);
        exit(ERR_INTERNAL);
    }
    
    StrInitString(&new_token->string_data);
    clean_token(new_token);

    return new_token;
}

int str_to_int (t_token* token)
{
    char* endptr;
    int tmp = (int)strtol(token->string_data.data, &endptr, 10); //can do this becouse we dont check overflow of int
    if (*endptr != '\0') //should not happend, becouse automat checks :) But for sure.
    {
        return 1;
    }
    token->int_data = tmp;
    token->string_data.data[0] = '\0';
    token->string_data.pos = 0;
    return 0;
}

int str_to_double (t_token* token)
{
    char* endptr;
    double tmp = strtod(token->string_data.data, &endptr);
    if (*endptr != '\0') //should not happend, becouse automat checks :) But for sure.
    {
        return 1;
    }
    token->double_data = tmp;
    token->string_data.data[0] = '\0';
    token->string_data.pos = 0;
    return 0;
}

void push_char_back(int c)
{
    if (!isspace(c))
    {
        ungetc(c, source);
    }

}

void keywords_check(t_token* token)
{
    if (strcmp(token->string_data.data, "boolean") == 0)
    {
        token->state = S_BOOL;
    }
    else if (strcmp(token->string_data.data, "break") == 0)
    {
        token->state = S_BREAK;
    }
    else if (strcmp(token->string_data.data, "class") == 0)
    {
        token->state = S_CLASS;
    }
    else if (strcmp(token->string_data.data, "continue") == 0)
    {
        token->state = S_CONTINUE;
    }
    else if (strcmp(token->string_data.data, "do") == 0)
    {
        token->state = S_DO;
    }
    else if (strcmp(token->string_data.data, "double") == 0)
    {
        token->state = S_DOUBLE_KEY;
    }
    else if (strcmp(token->string_data.data, "else") == 0)
    {
        token->state = S_ELSE;
    }
    else if (strcmp(token->string_data.data, "false") == 0)
    {
        token->state = S_FALSE;
    }
    else if (strcmp(token->string_data.data, "for") == 0)
    {
        token->state = S_FOR;
    }
    else if (strcmp(token->string_data.data, "if") == 0)
    {
        token->state = S_IF;
    }
    else if (strcmp(token->string_data.data, "int") == 0)
    {
        token->state = S_INT_KEY;
    }
    else if (strcmp(token->string_data.data, "return") == 0)
    {
        token->state = S_RETURN;
    }
    else if (strcmp(token->string_data.data, "String") == 0)
    {
        token->state = S_STRING;
    }
    else if (strcmp(token->string_data.data, "static") == 0)
    {
        token->state = S_STATIC;
    }
    else if (strcmp(token->string_data.data, "true") == 0)
    {
        token->state = S_TRUE;
    }
    else if (strcmp(token->string_data.data, "void") == 0)
    {
        token->state = S_VOID;
    }
    else if (strcmp(token->string_data.data, "while") == 0)
    {
        token->state = S_WHILE;
    }
}

