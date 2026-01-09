CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = client.c interface.c
OBJS = $(SRCS:.c=.o)
DEPS = client.h


TARGET = clientProgram


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^


run: $(TARGET)
	./$(TARGET)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)