CC = gcc
CFLAGS = -Wall -pedantic -ansi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

NUMBERSmake: FIND_BITS.o
	$(CC) -o  find_bits FIND_BITS.o      $(CFLAGS)

run: find_bits
	./find_bits

valgrind: find_bits
	valgrind --leak-check=full --track-origins=yes ./find_bits
clean:
	rm -f *.o find_bitsS
