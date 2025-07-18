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

#define ROOM_COUNT 5
#define BACKLOG 10
#define MAX_CLIENTS 50
#define PORT "3490"

extern room_s *room1;
extern int    g_current_clients;

void *handle_client(void *arg);

