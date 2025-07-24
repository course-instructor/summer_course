CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = preprocessor.c 
OBJS = $(SRCS:.c=.o)
DEPS = preprocessor.h

ARGS = TEST/a.c


TARGET = preprocessorProgram


all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

run: $(TARGET)
	./$(TARGET) $(ARGS)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) $(ARGS)

clean:
	rm -f $(OBJS) $(TARGET)