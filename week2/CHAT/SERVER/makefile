CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = server.c requestHandling.c
OBJS = $(SRCS:.c=.o)
DEPS = set.h


TARGET = setprog


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)