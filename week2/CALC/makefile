CC = gcc
CFLAGS = -Wall -Wextra -g -ansi -std=c99

SRCS = myset.c set.c
OBJS = $(SRCS:.c=.o)
DEPS = set.h myset.h
PROGRAM_ARGS = file.txt

TARGET = setprog

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) 

runFile: $(TARGET)
	./$(TARGET) $(PROGRAM_ARGS)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)