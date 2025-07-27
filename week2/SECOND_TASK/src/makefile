# Makefile

CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11 -g -D_POSIX_C_SOURCE=200112L -I. -Iserver -Iclient
LDFLAGS  := -pthread

SERVER_SRCS := server/server.c \
               server/s_handle_request.c \
               server/s_rooms.c \
               common.c

CLIENT_SRCS := client/client.c \
               client/c_requests.c \
               client/c_responce.c \
               common.c

SERVER_BIN := server/server
CLIENT_BIN := client/client

.PHONY: all clean

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(CLIENT_BIN): $(CLIENT_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
