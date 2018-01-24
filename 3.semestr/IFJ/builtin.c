/********************************************************************/
/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
/*Jména řešitelů: Martin Ivančo, Filip Januš, Vladimír Jeřábek      */
/*                Petr Jůda, Jakub Klemens                          */
/*Varianta zadání: a/1/I                                            */
/********************************************************************/

#include "builtin.h"

/**
 * Read integer from standard input.
 *
 * @return   integer from input, or -1 if the input is not integer
 */
int readInt()
{
	char *s = readString();
	int len = strlen(s);
	int i;

	if (len == 0)
	{
		free(s);
		return -1;
	}

	for (i = 0; i < len; i++)
	{
		if (isdigit(s[i]) == 0)
		{
			free(s);
			return -1;
		}
	}

	int res = strtol(s, NULL, 10);
	free(s);

	return res;
}

/**
 * Read double from standard input.
 *
 * @return   double from input, or -1 if the input is not double
 */
double readDouble()
{
	char *s = readString();
	int len = strlen(s);
	int i = 0;
	double res;

	if (len == 0)
		return -1;

	while (isdigit(s[i]) != 0)              //whole part
	{
		i += 1;

		if (i == len)
		{
			res = strtod(s, NULL);
			free(s);
			return res;
		}
	}

	if (s[i] == '.')                        //decimal part
	{
		i += 1;

		if (i == len)
		{
			free(s);
			return -1;
		}

		while (isdigit(s[i]) != 0)
		{
			i += 1;

			if (i == len)
			{
				res = strtod(s, NULL);
				free(s);
				return res;
			}
		}
	}

	if ((s[i] == 'e') || (s[i] == 'E'))     //exponent part
	{
		i += 1;

		if (i == len)
		{
			free(s);
			return -1;
		}

		if ((s[i] == '+') || (s[i] == '-'))
		{
			i += 1;

			if (i == len)
			{
				free(s);
				return -1;
			}
		}

		while (isdigit(s[i]) != 0)
		{
			i += 1;

			if (i == len)
			{
				res = strtod(s, NULL);
				free(s);
				return res;
			}
		}
	}

	free(s);                                //contains bad characters
	return -1;
}

/**
 * Read string from standard input.
 *
 * @return   string from input until \n or EOF
 */
char * readString()
{
	int c;
	char *s = (char *) malloc(8 * sizeof(char));
	if (s == NULL)
	{
		fprintf(stderr, "Error at allocation of string!\n");
		exit(ERR_INTERNAL);
	}
	int len = 0;
	int max = 7;

	while (1)
	{
		c = getchar();

		if ((c == EOF) || (c == '\n'))
		{
			if (len == 0)
				s[len] = '\0';

			return s;
		}

		if(len == max)
		{
			max = 2*max+1;
			s = (char *) realloc(s, (max+1) * sizeof(char));
			if (s == NULL)
			{
				fprintf(stderr, "Error at allocation of string!\n");
				exit(ERR_INTERNAL);
			}
		}

		s[len] = c;
		s[len+1] = '\0';

		len += 1;
	}
}

/**
 * Print a term or concatenation on stdout.
 */
void print(symbol_variable *ptr)
{
	switch (ptr->type)
	{
		case variable_integer:
			printf("%d", ptr->value.value_integer);
			break;

		case variable_double:
			printf("%g", ptr->value.value_double);
			break;

		case variable_string:
			printf("%s", ptr->value.value_string);
			break;

		default: //should not happend parser checks?
			fprintf(stderr, "Error - unknown type of variable!\n");
			exit(ERR_INTERNAL);
	}

}

/**
 * Get length of string.
 *
 * @return   length of given string, if pointer is NULL -1
 */
int length(char *s)
{
	if (s == NULL)
		return -1;

	return strlen(s);
}

/**
 * Get substring of string by first position and length.
 *
 * @return   substring of given string
 */
char * substr(char *s, int i, int n)
{
    char *res = NULL;
    // i < 0 should not happend because lexical analysis not support read of negative int
    if (i < 0 || (i + n) > length(s) )
    {
        return res;
    }
    
	res = (char *) malloc((n+1)*sizeof(char));
	if (res == NULL)
	{
		fprintf(stderr, "Error at allocation of string!\n");
		exit(ERR_INTERNAL);
	}

	strncpy(res, (s+i), n);
	return res;
}

/**
 * Compare two strings.
 *
 * @return   0 if they are equal, 1 if s1 is bigger, -1 if s2 is bigger
 */
int compare_string(char *s1, char *s2)
{
	int res = strcmp(s1, s2);

	if (res < 0)
		return -1;

	if (res > 0)
		return 1;

	else
		return 0;
}

/**
 * Find substring in string.
 *
 * @return   position of the substring in string, -1 if not found
 */
int find(char *s, char *search)
{
	int *f = kmp_graph(search);
	int res = kmp_match(s, search, f);
	free(f);
	return res;
}

/**
 * Sort characters of string.
 *
 * @return   sorted string.
 */
char * sort(char *s)
{
	int len = strlen(s);
	char *res = (char *) malloc((len+1) * sizeof(char));
	if (res == NULL)
	{
		fprintf(stderr, "Error at allocation of string!\n");
		exit(ERR_INTERNAL);
	}
	strcpy(res, s);

	qs_sort(res, 0, (len-1));
	return res;
}
