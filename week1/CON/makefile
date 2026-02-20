CC = gcc
CFLAGS = -Wall -pedantic -ansi

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

con: CON.o
	$(CC) -o  con CON.o      $(CFLAGS)

run: con
	./con

valgrind: con
	valgrind --leak-check=full --track-origins=yes ./con
clean:
	rm -f *.o con
