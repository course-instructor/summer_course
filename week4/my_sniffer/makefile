CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = my_sniffer.c src/sniffer.c
OBJS = $(SRCS:.c=.o)
DEPS = include/my_sniffer.h


TARGET = sniffer_program


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^


run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
