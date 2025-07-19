#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#include "s_handle_request.h"
#include "common.h"

room_s *room1;
int     g_current_clients = 0;

void *handle_client(void *arg)
{
    printf("handling new client\n");
    client_ptr_t client = (client_ptr_t)arg;
    // send(client->sockfd, "Welcome to the chat!\n", 22, 0);

    while (get_message(client));

    printf("client out\n");
    close(client->sockfd);
    pthread_mutex_lock(&clients_mutex);
    g_current_clients--;
    rem_client(room1->clients, client);
    pthread_mutex_unlock(&clients_mutex);
    free(client);
    return NULL;
}

int main(void)
{
    int sockfd, new_fd, rv, yes = 1;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
        return 1;

    for (p = servinfo; p; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
    if (!p) exit(1);
    listen(sockfd, BACKLOG);

    g_rooms[0].clients = innit_clients();
    room1 = &g_rooms[0];
    printf("wiating for connections\n");
    while (1)
    {
        sin_size = sizeof their_addr;
        new_fd   = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *)&their_addr),
                  s, sizeof s);

        pthread_mutex_lock(&clients_mutex);
        if (g_current_clients >= MAX_CLIENTS)
        {
            pthread_mutex_unlock(&clients_mutex);
            printf("clients over %d\n", MAX_CLIENTS);
            close(new_fd);
            continue;
        }
        g_current_clients++;
        pthread_mutex_unlock(&clients_mutex);

        client_ptr_t client = malloc(sizeof(*client));
        client->sockfd = new_fd;
        client->addr = their_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client) == 0)
            pthread_detach(tid);
        else
        {
            close(new_fd);
            free(client);
            pthread_mutex_lock(&clients_mutex);
            g_current_clients--;
            pthread_mutex_unlock(&clients_mutex);
        }
    }
    return 0;
}
