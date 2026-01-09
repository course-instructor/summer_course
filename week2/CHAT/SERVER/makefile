CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = server.c requestHandling.c
OBJS = $(SRCS:.c=.o)
DEPS = server.h


TARGET = serverProgram


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)