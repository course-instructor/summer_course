# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -g -D_POSIX_C_SOURCE=200112L
LDFLAGS = -pthread

# Directories
SERVER_DIR   = server
CLIENT_DIR   = client

# Include paths
INCLUDES     = -I. -I$(SERVER_DIR) -I$(CLIENT_DIR)

# Source file lists
# Collect all .c in server directory plus common.c
SERVER_SRCS  = $(wildcard $(SERVER_DIR)/*.c) common.c
# Collect all .c in client directory plus common.c
CLIENT_SRCS  = $(wildcard $(CLIENT_DIR)/*.c) common.c

# Binaries
SERVER_BIN   = $(SERVER_DIR)/server
CLIENT_BIN   = $(CLIENT_DIR)/client

# Default target: build both
.PHONY: all
all: $(SERVER_BIN) $(CLIENT_BIN)

# Build server from all sources
$(SERVER_BIN): $(SERVER_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

# Build client from all sources
$(CLIENT_BIN): $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

# Run rules
.PHONY: run-server run-client
run-server: $(SERVER_BIN)
	./$(SERVER_BIN)

run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN) localhost

# Clean up binaries only
.PHONY: clean
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
