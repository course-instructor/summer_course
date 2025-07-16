# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g -D_POSIX_C_SOURCE=200112L

# Directories
SERVER_DIR   = server
CLIENT_DIR   = client

# Source files
SERVER_SRC   = $(SERVER_DIR)/server.c
CLIENT_SRC   = $(CLIENT_DIR)/client.c

# Object files
SERVER_OBJS  = $(SERVER_SRC:.c=.o)
CLIENT_OBJS  = $(CLIENT_SRC:.c=.o)

# Targets (binaries)
SERVER_BIN   = $(SERVER_DIR)/server
CLIENT_BIN   = $(CLIENT_DIR)/client

# Default target: build both
.PHONY: all
all: $(SERVER_BIN) $(CLIENT_BIN)

# Link server
$(SERVER_BIN): $(SERVER_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Link client
$(CLIENT_BIN): $(CLIENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile rule for .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run rules
.PHONY: run-server run-client
run-server: $(SERVER_BIN)
	./$(SERVER_BIN)

run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN) localhost

# Clean up
.PHONY: clean
clean:
	rm -f $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER_BIN) $(CLIENT_BIN)
