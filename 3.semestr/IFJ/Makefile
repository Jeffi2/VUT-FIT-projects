
#/********************************************************************/
#/*Projekt: Implementace interpretu podmnožiny jazyka JAVA - IFJ16   */
#/*Loginy řešitelů: xivanc03, xjanus08, xjerab21, xjudap00, xkleme11 */
#/*Jména řešitelů: Martin Invančo, Filip Januš, Vladimír Jeřábek     */
#/*                Petr Jůda, Jakub Klemens                          */
#/*Varianta zadání: a/1/I                                            */
#/********************************************************************/

NAME=bin
OBJFILES=sym_table.o call_function.o symbol.o ial.o scaner.o lltable.o parser.o expressions.o constants.o string.o stack.o interpret.o generate.o recursion.o builtin.o main.o


CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
#-lm

# univerzalni pravidlo pro generovani vsech objektovych souboru
%.o : %.c
	$(CC) $(CFLAGS) -c $<

# Startovaci pravidlo
all: $(NAME)

# Slinkovani vsech objektovych souboru do jednoho spustitelneho programu .
$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) $(OBJFILES) -o $@

valg:
	valgrind --leak-check=full --show-leak-kinds=all ./$(NAME) $(TEST_FILE)

# Pravidlo pro vycisteni adresare, kde jsou zdrojove kody
clean:
	rm -rf $(OBJFILES) $(NAME)

# Spusteni regresnich testu nad prelozenym interpretem
test: $(NAME)
	./test.sh
