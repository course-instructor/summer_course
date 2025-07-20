#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "server.h"
#include "s_handle_request.h"
#include "common.h"

int g_current_clients = 0; //how many clients are connected

/**
 * @brief function getting messages from a client and responding to them acordingly
 *
 * @param arg void * (client_ptr_t) the client to listen to
 * @return void*
 */
void *handle_client(void *arg)
{
    printf("handling new client\n");
    client_ptr_t client = (client_ptr_t)arg;

    while (get_message(client));

    printf("client out\n");
    close(client->sockfd);
    pthread_mutex_lock(&clients_mutex);
    g_current_clients--;



    room_rem_client(& g_rooms[client->room_index],client);


    pthread_mutex_unlock(&clients_mutex);
    free(client);
    return NULL;
}


void * get_input(void *)
{

    int number;
    char text [MESSAGE_LENGTH];
    char string[MESSAGE_LENGTH];

    while(1)
    {
        printf("enter room index, and message to send:\t");
        fflush(stdout);

        fgets(text,MESSAGE_LENGTH,stdin);

        char *line_end = strchr(text,'\n');
        if (line_end)
        {
            *line_end = '\0';
        }


        sscanf(text, "%d %s", &number, string);

        if(number <0 || number >= ROOM_COUNT)
        {
            printf("room number must be 0-%d\n", ROOM_COUNT - 1);
            continue;
        }

        broadcast(&g_rooms[number], NULL, NULL, string);

    }

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
    {
        return 1;
    }

    for (p = servinfo; p; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            continue;
        }
        break;
    }
    freeaddrinfo(servinfo);
    if (!p)
    {
        exit(1);
    }

    innit_g_rooms();

    pthread_t ui;
    pthread_create(& ui,NULL, get_input,&sockfd);


    listen(sockfd, BACKLOG);


    printf("waiting for connections\n");



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
