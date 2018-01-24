CFlAGS= -std=c++11 -Wall -Wextra -pedantic 
CC= g++

all:ims

ims:main.cpp maslo.cc baleni.cc maslo.h baleni.h zarizeni.h zarizeni.cc
	$(CC) $(CFLAGS) main.cpp maslo.cc baleni.cc zarizeni.cc -o mlekarna -lsimlib -lm

run:
	./mlekarna

clean:
	rm mlekarna
