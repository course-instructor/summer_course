# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -ansi -g

# Folders
SRC_DIR = src
INC_DIR = headers
OBJ_DIR = obj
BIN_DIR = bin

# Files
SRC = $(SRC_DIR)/tree.c
OBJ = $(OBJ_DIR)/tree.o
TARGET = $(BIN_DIR)/tree_program

# Default target
all: $(TARGET)

# Link the final executable
$(TARGET): $(OBJ) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source to object file
$(OBJ): $(SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@


# Clean build artifacts
clean:
	rm -rf
