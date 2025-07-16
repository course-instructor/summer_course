#include <stdio.h>
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

#include "common.h"

#define PORT "3490"
#define BACKLOG 10
#define MAX_CLIENTS 50
#define MAX_DATA_SIZE 100

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct 
{
    int sockfd;
    struct sockaddr_storage addr;
} *client_ptr_t;

typedef struct clients_node 
{
    client_ptr_t client;
    struct clients_node *next;
} *client_list_t;


client_list_t innit_clients() 
{
    client_list_t head = (client_list_t)malloc(sizeof(* head));
    if (!head) 
    {
        perror("malloc innit_clients head");
        exit(EXIT_FAILURE);
    }
    head -> client = NULL; 
    head -> next = NULL;
    return head;
}




void add_client(client_list_t head, client_ptr_t new_client)
{
    client_list_t current = head;
    while (current -> next)
    {
        if (current -> next -> client && current -> next -> client -> sockfd == new_client -> sockfd) 
        {
            printf("Server: Client %d already in list.\n", new_client -> sockfd);
            return;
        }
        current = current->next;
    }

    client_list_t new_node = (client_list_t) malloc (sizeof(struct clients_node));
    if (!new_node)
    {
        perror("malloc add_client node");
        return;
    }
    new_node -> client = new_client; 
    new_node -> next = NULL;

    current -> next = new_node; 
    printf("Server: Client %d added to list.\n", new_client -> sockfd);
}

int rem_client(client_list_t head, client_ptr_t  remove_client)
{
    int ret = 0;

    client_list_t current = head;
    client_list_t prev = NULL;

    if(head -> client == remove_client)
    {
        head = head -> next;
        free(current);
    }

    while (current) 
    {
        if (current -> client == remove_client)
        {
            prev -> next = current -> next;
            free(current);
            ret = 1; // remove_client was found...
            break;
        }
        prev = current;
        current = current -> next;
    }
    return(ret);
}

void broadcast(client_list_t head, client_ptr_t ignore, const char *payload)
{
    pthread_mutex_lock(& clients_mutex);
    char sender[INET6_ADDRSTRLEN];
    inet_ntop(ignore -> addr.ss_family, get_in_addr((struct sockaddr*) & ignore -> addr), sender, sizeof sender);

    char out[1200];
    int len = snprintf(out, sizeof out, "%s: %s\n", sender, payload);

    for (client_list_t cur = head -> next; cur; cur = cur->next)
    {
        if (cur -> client != ignore)
        {
            send(cur -> client -> sockfd, out, len, 0);
        }
    }
    pthread_mutex_unlock(& clients_mutex);
    fflush(stdout);
}

client_list_t room1;

int g_current_clients = 0;




void *handle_client(void *arg)
{
    client_ptr_t client = (client_ptr_t )arg;
    char buf[1024];
    int bytes_received;

    send(client->sockfd, "Welcome to the chat!\n", 22, 0);

    int connected_flag = 0;

    while ((bytes_received = recv(client->sockfd, buf, sizeof(buf) - 1, 0)) > 0)
    {
        buf[bytes_received] = '\0';

        printf("Server: Received from client: %s\n", buf); 
        fflush(stdout);


        if(connected_flag)
        {
            broadcast(room1, client, buf);
        }

        char response_msg[1024];
        snprintf(response_msg, sizeof(response_msg), "Server received: '%s'", buf); // Formulate response
        if(strcmp(buf,"end") == 0)
        {
            printf("client ended the connection");
            send(client->sockfd, "goodbye\0", strlen(response_msg), 0);
            break;
        }

        

        else if(strcmp(buf,"connect") == 0)
        {
            connected_flag = 1;
            pthread_mutex_lock(& clients_mutex);
            add_client(room1, client);
            pthread_mutex_unlock(& clients_mutex);
        }

        else if (send(client->sockfd, response_msg, strlen(response_msg), 0) == -1) 
        {
            perror("send response to client");
        }
    }

    printf("Client disconnected.\n");

    close(client->sockfd);

    pthread_mutex_lock(&clients_mutex);
    g_current_clients--;
    rem_client(room1, client);
    pthread_mutex_unlock(&clients_mutex);
    free(client);

    pthread_exit(NULL);
}

int main(void) 
{
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    char s[INET6_ADDRSTRLEN];
    int yes = 1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // Use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through results and bind to the first available
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) 
        {
            perror("setsockopt");
            return 1;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) 
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) 
    {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    room1 =  innit_clients();

    while (1) 
    {
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) 
        {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
        printf("server: got connection from %s\n", s);

        // check and handle if the clients are over the allowd amount
        pthread_mutex_lock(&clients_mutex);
        if (g_current_clients >= MAX_CLIENTS) 
        {
            pthread_mutex_unlock(&clients_mutex);
            const char *msg = "currently server full\n";
            send(new_fd, msg, strlen(msg), 0);
            close(new_fd);
            continue;
        }

        g_current_clients++;
        pthread_mutex_unlock(&clients_mutex);

        //add another client

        client_ptr_t client = malloc(sizeof(* client));
        client->sockfd = new_fd;
        client->addr = their_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client) != 0) 
        {
            perror("pthread_create");
            close(new_fd);
            free(client);
            pthread_mutex_lock(&clients_mutex);
            g_current_clients--;
            pthread_mutex_unlock(&clients_mutex);
        }

        pthread_detach(tid); 
    }

    return 0;
}
