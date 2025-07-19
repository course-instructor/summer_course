#include "s_rooms.h"
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>    // for inet_ntop
#include "common.h"       // for get_in_addr

room_s g_rooms[ROOM_COUNT];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

client_list_t innit_clients(void)

{
    client_list_t head = malloc(sizeof(clients_node_t));
    head->client = NULL;
    head->next   = NULL;
    return head;
}

void add_client(client_list_t head, client_ptr_t new_client)
{
    client_list_t cur = head;
    while (cur->next)
    {
        if (cur->next->client == new_client) return;
        cur = cur->next;
    }
    client_list_t node = malloc(sizeof(clients_node_t));
    if (!node) { perror("malloc"); return; }
    node->client = new_client;
    node->next   = NULL;
    cur->next    = node;
}

int rem_client(client_list_t head, client_ptr_t rem)
{
    client_list_t prev = head, cur = head->next;
    while (cur) {
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

void broadcast(room_s *room, client_ptr_t ignore, message_s * message)
{
    pthread_mutex_lock(& room->mutex);

    for (client_list_t cur = room->clients->next; cur; cur = cur->next)
    {
        if (cur->client != ignore)
        {
            send_message(cur->client->sockfd, message);
        }
    }
    pthread_mutex_unlock(& room->mutex);
}


void innit_g_rooms(void)
{
    for(int i = 0; i <ROOM_COUNT; i++)
    {
        g_rooms[i].clients = innit_clients();
        pthread_mutex_init(&g_rooms[i].mutex, NULL);
        sprintf(g_rooms[i].name, "room %d", i);
    }
}