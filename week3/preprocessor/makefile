CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99


SRCS = main.c src/preprocessor.c 
OBJS = $(SRCS:.c=.o)
DEPS = include/preprocessor.h


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