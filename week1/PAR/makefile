CC=gcc
CFLAGS= -Wall -pedantic -ansi
DEPS=stack.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

PARmake: PAR.o
	$(CC) -o PARmake PAR.o
