CC=g++                              
CFLAGS=-std=c++11 -Wall -Wextra

all: main.o ParamsParser.o Pop3.o Database.o 
	$(CC) main.o ParamsParser.o Database.o Pop3.o  -o popcl -lcrypto -lssl


%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf ParamsParser.o main.o Pop3.o Database.o popcl