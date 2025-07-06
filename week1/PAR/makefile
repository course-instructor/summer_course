CC = gcc
CFLAGS = -Wall -pedantic -ansi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

NUMBERSmake: PAR.o
	$(CC) -o  par PAR.o      $(CFLAGS)

run: par
	./par

valgrind: par
	valgrind --leak-check=full --track-origins=yes ./par
clean:
	rm -f *.o par
