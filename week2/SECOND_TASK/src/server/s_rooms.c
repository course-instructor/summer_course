#include "s_rooms.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>    // for inet_ntop
#include "common.h"       // for get_in_addr

room_s g_rooms[ROOM_COUNT];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief creates and initializes a client_list_t
 *
 * @return client_list_t the newly created client linked list
 */
client_list_t innit_clients(void)

{
    client_list_t head = malloc(sizeof(clients_node_t));
    head->client = NULL;
    head->next   = NULL;
    return head;
}

/**
 * @brief adds a client to a client linked list if it isnt already there, the first one will be added to the head->next and not to head->client so head->client will be NULL
 *
 * @param head the start of the client linked list
 * @param new_client the client to be added to the linked list
 */
void add_client(client_list_t head, client_ptr_t new_client)
{
    client_list_t cur = head;
    while (cur->next)
    {
        if (cur->next->client == new_client)
            return;
        cur = cur->next;
    }
    client_list_t node = malloc(sizeof(clients_node_t));
    if (!node)
    {
        perror("malloc");
        return;
    }
    node->client = new_client;
    node->next   = NULL;
    cur->next    = node;
    printf("added client\n");
}

/**
 * @brief removes client from linked listif exists
 *
 * @param head start of client linked list
 * @param rem
 * @return int was the client removed? (1,0)
 */
int rem_client(client_list_t head, client_ptr_t rem)
{
    client_list_t prev = head, cur = head->next;
    while (cur)
    {
        if (cur->client == rem)
        {
            prev->next = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur  = cur->next;
    }
    return 0;
}

/**
 * @brief adds client to room and locks the room
 *
 * @param room room to add client
 * @param client client to add to the room
 */
void room_add_client(room_s *room, client_ptr_t client)
{
    pthread_mutex_lock(& room->mutex);
    add_client(room->clients, client);
    pthread_mutex_unlock(& room->mutex);
}

/**
 * @brief removes client from room and locks the room
 *
 * @param room room to remove client from
 * @param client client to remove from room
 */
void room_rem_client(room_s *room, client_ptr_t client)
{
    pthread_mutex_lock(& room->mutex);
    rem_client(room->clients, client);
    pthread_mutex_unlock(& room->mutex);
}

/**
 * @brief send message to everyone in a room except one client
 *
 * @param room room that the message will be sent in
 * @param ignore a client to ignore (the client that sent the message)
 * @param name the name that the client used to send the message
 * @param str the message to send
 */
void broadcast(room_s *room, client_ptr_t ignore, const char* name, const char* str)
{

    message_s message;
    message.request_num = name ? MESSAGE_FROM_CLIENT : MESSAGE_FROM_SERVER;

    const char * with_name [] = { name, str, NULL};
    const char * without_name [] = { str, NULL};

    message.params = name ? with_name : without_name;

    pthread_mutex_lock(& room->mutex);

    for (client_list_t cur = room->clients; cur; cur = cur->next)
    {
        if (cur->client && cur->client != ignore)
        {
            send_message(cur->client->sockfd, & message);
        }
    }
    pthread_mutex_unlock(& room->mutex);
}

/**
 * @brief initialize g_rooms and give them names
 *
 */
void innit_g_rooms(void)
{
    for(int i = 0; i <ROOM_COUNT; i++)
    {
        g_rooms[i].clients = innit_clients();
        pthread_mutex_init(&g_rooms[i].mutex, NULL);
        sprintf(g_rooms[i].name, "room %d", i);
    }
}