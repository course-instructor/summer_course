CC = gcc
CFLAGS = -Wall -pedantic -ansi
DEPS = set.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

MYSETmake: myset.o set.o
	$(CC) -o myset myset.o set.o $(CFLAGS)

run: myset
	./myset

valgrind:
	valgrind --leak-check=full --track-origins=yes ./myset 


clean:
	rm -f *.o myset
