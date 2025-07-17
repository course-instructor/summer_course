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
    client_ptr_t client = (client_ptr_t)arg;
    char         buf[1024];
    int          bytes_received;
    send(client->sockfd, "Welcome to the chat!\n", 22, 0);
    int connected_flag = 0;

    while ((bytes_received = recv(client->sockfd, buf, sizeof(buf) - 1, 0)) > 0)
    {
        buf[bytes_received] = '\0';
        printf("Server: Received from client: %s\n", buf);
        fflush(stdout);

        if (connected_flag)
            broadcast(room1, client, buf);

        char response_msg[1024];
        snprintf(response_msg, sizeof(response_msg), "Server received: '%s'", buf);

        if (strcmp(buf, "end") == 0)
        {
            send(client->sockfd, "goodbye\0", strlen(response_msg), 0);
            break;
        }
        else if (strcmp(buf, "connect") == 0)
        {
            connected_flag = 1;
            pthread_mutex_lock(&clients_mutex);
            add_client(room1->clients, client);
            pthread_mutex_unlock(&clients_mutex);
        }
        else
            send(client->sockfd, response_msg, strlen(response_msg), 0);
    }

    close(client->sockfd);
    pthread_mutex_lock(&clients_mutex);
    g_current_clients--;
    rem_client(room1->clients, client);
    pthread_mutex_unlock(&clients_mutex);
    free(client);
    pthread_exit(NULL);
}

int main(void)
{
    int                 sockfd, new_fd, rv, yes = 1;
    struct addrinfo     hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t           sin_size;
    char                s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

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
    room1             = &g_rooms[0];

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
            close(new_fd);
            continue;
        }
        g_current_clients++;
        pthread_mutex_unlock(&clients_mutex);

        client_ptr_t client = malloc(sizeof(*client));
        client->sockfd = new_fd;
        client->addr   = their_addr;

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
