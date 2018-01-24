CC=g++                               # překladač jazyka C
CFLAGS=-std=c++11 


all:server client

server:ftrestd.cpp  
	$(CC) $(CFLAGS) ftrestd.cpp -o ftrestd
client:ftrest.cpp
	$(CC) $(CFLAGS) ftrest.cpp -o ftrest
