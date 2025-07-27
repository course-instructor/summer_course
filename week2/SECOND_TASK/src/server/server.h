#pragma once

#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "s_rooms.h"

#define BACKLOG MAX_CLIENTS //number of clients that can wait after the 3 way handshake
#define MAX_CLIENTS 50 //max allowed number of clients

extern room_s  g_rooms[ROOM_COUNT];
extern int    g_current_clients;

void *handle_client(void *arg);

