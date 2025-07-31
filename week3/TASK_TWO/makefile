# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -ansi -g -D_DEFAULT_SOURCE

# Source and headers
SRC = src/preprocessor.c
INCLUDES = -Iheaders

# Executable
TARGET = preprocessor

# Default rule
all: $(TARGET)

# Build the executable directly
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC)

# Clean
clean:
	rm -f $(TARGET)

.PHONY: all clean
