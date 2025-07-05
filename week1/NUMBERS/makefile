CC=gcc
CFLAGS= -Wall -pedantic -ansi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

NUMBERSmake: NUMBERS.o
	$(CC) -o NUMBERSmake NUMBERS.o
