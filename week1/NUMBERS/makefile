CC = gcc
CFLAGS = -Wall -pedantic -ansi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

NUMBERSmake: NUMBERS.o
	$(CC) -o  numbers NUMBERS.o      $(CFLAGS)

run: numbers
	./numbers

valgrind: numbers
	valgrind --leak-check=full --track-origins=yes ./numbers
clean:
	rm -f *.o numbers
