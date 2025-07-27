# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -ansi -pedantic -std=c99 -g

# Source files (replace these with your actual filenames)
SRCS = myset.c set.c
OBJS = $(SRCS:.c=.o)
DEPS = set.h

# Output binary
TARGET = setprog

# Default rule
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
