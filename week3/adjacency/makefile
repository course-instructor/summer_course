CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = main.c src/adjacency.c
OBJS = $(SRCS:.c=.o)
DEPS = ./header/server.h


TARGET = adjacencyProgram


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)