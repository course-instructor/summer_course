#pragma once
#include "common.h"
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>



#define ROOM_COUNT 5 //how many rooms there are in g_rooms

#define NAME_LENGTH 20

/// @brief clients linked list
typedef struct clients_node
{
    client_ptr_t client; //current client value
    struct clients_node *next; //pointer to next node in the linked list
} clients_node_t, *client_list_t;

/// @brief room struct where a u can send a broadcast message
typedef struct room_s
{
    client_list_t clients; //linked list of clients in the room
    pthread_mutex_t mutex; //room mutex to prevent race conditions when clients operate with the room
    char name [NAME_LENGTH]; //the rooms name that will be sent on a request of the list of rooms
} room_s;


extern room_s g_rooms[ROOM_COUNT];
extern pthread_mutex_t clients_mutex;

client_list_t innit_clients(void);

void room_add_client(room_s *room, client_ptr_t client);
void room_rem_client(room_s *room, client_ptr_t client);

void broadcast(room_s *room, client_ptr_t ignore, const char * name, const char * str);
void innit_g_rooms(void);