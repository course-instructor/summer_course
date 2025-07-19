#pragma once
#include "common.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>



#define ROOM_COUNT 5





typedef struct clients_node {
    client_ptr_t          client;
    struct clients_node  *next;
} clients_node_t, *client_list_t;

typedef struct room_s {
    client_list_t    clients;
    pthread_mutex_t  mutex;
} room_s;

extern room_s          g_rooms[ROOM_COUNT];
extern pthread_mutex_t clients_mutex;

client_list_t innit_clients(void);
void           add_client(client_list_t head, client_ptr_t new_client);
int            rem_client(client_list_t head, client_ptr_t remove_client);
void           broadcast(room_s *room, client_ptr_t ignore, const char *payload);
