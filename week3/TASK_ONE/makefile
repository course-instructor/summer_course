# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -ansi -g

# Source and headers
SRC = src/tree.c
INCLUDES = -Iheaders

# Executable
TARGET = tree_program

# Default rule
all: $(TARGET)

# Build the executable directly
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC)

# Clean
clean:
	rm -f $(TARGET)

.PHONY: all clean
