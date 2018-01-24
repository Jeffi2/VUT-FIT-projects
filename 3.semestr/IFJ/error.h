/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#define OK              0
#define LEX_ERR        -1
#define SYNTAX_ERR     -2
#define SEM_ERR        -3

#define TYPE_ERR       -4
#define OTHER_SEM_ERR  -6
#define IO_ERR         -7
#define NONINI_ERR     -8
#define DIV_ZERO_ERR   -9
#define ERR_UNKNOWN    -10

#define ERR_INTERNAL 99
