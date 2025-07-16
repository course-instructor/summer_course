#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>


#include "common.h"


#define PORT "3490"         // Port number for connection

enum e_highrarchy { NOT_REGISTERD, LOGGED_IN, IN_ROOM };

char ** INP_MESSAGES =
{
"sign up (enter 100), or log in (enter 101):\t",
"get a list of the rooms (enter 102), or choose a room to enter (103):\t",
"send a message in the room (enter 104), or exit room (enter 105):\t"
};



enum e_highrarchy g_level = NOT_REGISTERD;
pthread_mutex_init(g_level);



void get_name_and_pass(char * name, char * password)
{
    printf("enter username: \t");
    fgets(name, MESSAGE_LENGTH, stdin);
    int name_length = strchr(name,'\n');
    name[name_length] = '\0';

    printf("enter password: \t");
    int pass_len = fgets(password, MESSAGE_LENGTH, stdin);
    int pass_length = strchr(name,'\n');
    name[pass_length] = '\0';
}



void get_input(int sockfd)
{
    int number;
    char text [MESSAGE_LENGTH];
    enum e_highrarchy curr_levl = NOT_REGISTERD;

    while(1)
    {
        pthread_mutex_lock(g_level);
        if(g_level != curr_levl)
        {
            curr_levl = g_level;
            printf(INP_MESSAGES[g_level]);
        }
        pthread_mutex_unlock(g_level);

        
        scanf("%d", &number);


        switch (number)
        {
            case SIGN_UP:
                char name [MESSAGE_LENGTH] ;
                char password [MESSAGE_LENGTH] ;

                name[0] = '\0';
                password[0] = '\0';

                get_name_and_pass(name,password);
                send_signup_message(sockfd,name,password);
                break;
            
            // case LOG_IN:
            //     char name [MESSAGE_LENGTH] ;
            //     char password [MESSAGE_LENGTH] ;

            //     name[0] = '\0';
            //     password[0] = '\0';

            //     get_name_and_pass(name,password);
            //     send_signup_message
            
            default:
                break;
        }        
        


        char buf [MESSAGE_LENGTH];
        fgets(buf, MESSAGE_LENGTH, stdin);
        buf[strlen(buf) - 1] = '\0'; //handle the \n from fgets
    }
    

}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MESSAGE_LENGTH];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    // Check for correct command-line arguments.
    if (argc != 2) 
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets

    

    // Get server address information.
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) 
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through results and connect to the first available.
    for (p = servinfo; p != NULL; p = p->ai_next) 
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break; // Connection successful
    }

    // Handle connection failure.
    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    // Convert and print connected IP address.
    inet_ntop(p -> ai_family, get_in_addr((struct sockaddr*)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);

    pthread server_listn;
    pthread ui;

    pthread_create(& server_listn, get_message());
    pthread_create(& ui, get_input());

    freeaddrinfo(servinfo); // Free allocated memory

    buf[0] = '\0';
    
    
    close(sockfd); // Close the socket.
    return 0;
}