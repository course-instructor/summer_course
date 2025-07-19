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
#include <stdatomic.h>



#include "common.h"
#include  "c_request.h"




#define PORT "3490"         // Port number for connection

const char * INP_MESSAGES [] =
{
"sign up (enter 100), or log in (enter 101):\t",
"get a list of the rooms (enter 102), or choose a room to enter (103):\t",
"send a message in the room (enter 104), or exit room (enter 105):\t"
};

static const enum request_e ALLOWED_ACTIONS[][2] = {
    { SIGN_UP,       LOG_IN        },
    { LIST_OF_ROOMS, ENTER_ROOM    },
    { MESSAGE_ROOM,  EXIT_ROOM     }
};

_Atomic enum connection_e g_level = NOT_CONNECTED;



void get_name_and_pass(char * name, char * password)
{
    printf("enter username: \t");
    fflush(stdout);

    fgets(name, MESSAGE_LENGTH, stdin);

    char *line_end = strchr(name,'\n');
    if (line_end)
    {
        *line_end = '\0';
    }

    printf("enter password: \t");
    fflush(stdout);

    fgets(password, MESSAGE_LENGTH, stdin);
    line_end = strchr(password,'\n');
    if (line_end)
    {
        *line_end = '\0';
    }
}

void get_name_and_room(char * name, int * room_ptr)
{
    printf("enter username: \t");
    fflush(stdout);

    fgets(name, MESSAGE_LENGTH, stdin);

    char *line_end = strchr(name,'\n');
    if (line_end)
    {
        *line_end = '\0';

    }

    printf("enter room index to enter: \t");
    fflush(stdout);

    char temp[MESSAGE_LENGTH];

    fgets(temp,MESSAGE_LENGTH,stdin);

    sscanf(temp,"%d",room_ptr);
}


int check_action_permission(enum request_e request)
{
    int ret = 0;
    enum connection_e level = g_level; //so that it doesnt change in the middle of the if
    ret = (ALLOWED_ACTIONS[level][0] == request || ALLOWED_ACTIONS[level][1] == request);
    if(!ret)
    {
        printf("%s", INP_MESSAGES[level]);
        fflush(stdout);
    }

    return ret;
}

void * get_input(void * arg)
{
    int sockfd = *((int*) arg);

    int number;
    char text [MESSAGE_LENGTH];
    enum connection_e curr_level = IN_ROOM; //to ensure that it is different from the g_level initial value

    while(1)
    {
        if(g_level != IN_ROOM || g_level != curr_level)
        {
            curr_level = g_level;
            printf("%s", INP_MESSAGES[curr_level]);
            fflush(stdout);
        }

        fgets(text,MESSAGE_LENGTH,stdin);

        if(g_level == IN_ROOM)
        {
            char name [MESSAGE_LENGTH] ;

            name[0] = '\0';

            printf("enter username: \t");
            fflush(stdout);

            fgets(name, MESSAGE_LENGTH, stdin);

            char *line_end = strchr(name,'\n');
            if (line_end)
            {
                *line_end = '\0';
            }

            if(strcmp(text,"~\n")) //disconnect
            {
                send_leave_room_message(sockfd,name);
            }

            else
            {
                char str [MESSAGE_LENGTH];
                send_in_room_message(sockfd,name, str);
            }
        }

        else
        {

            number = -1;
            sscanf(text,"%d",&number);
            if(!check_action_permission(number)) //inapropriate protocol
            {
                continue;
            }

            char name [MESSAGE_LENGTH] ;
            char password [MESSAGE_LENGTH] ;

            name[0] = '\0';
            password[0] = '\0';


            switch (number)
            {
                case SIGN_UP:

                    get_name_and_pass(name,password);
                    send_signup_message(sockfd,name,password);
                    break;

                case LOG_IN:

                    get_name_and_pass(name,password);
                    send_login_message(sockfd,name,password);
                    break;
                case LIST_OF_ROOMS:
                    send_room_lst_message(sockfd);
                    break;

                case ENTER_ROOM:
                    int room;
                    get_name_and_room( name, &room);
                    send_enter_room_message(sockfd,name, room);
                default:
                    break;
            }

        }


    }

    return NULL;
}

void * handle_connection (void* arg)
{
    int sockfd = *((int *) arg);
    client_ptr_t server = malloc(sizeof(client_s));
    server->sockfd = sockfd;

    while(get_message(server));
    close(sockfd);
    free(server);
    printf("server disconected!\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    char * server_name;

    // Check for correct command-line arguments.
    if(argc == 1)
    {
        server_name = "localhost";
    }

    else if(argc == 2)
    {
        server_name = argv[1];
    }

    else
    {
        fprintf(stderr, "usage: %s [hostname]\n", argv[0]);
        exit(1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;     // Allow IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets



    // Get server address information.
    if ((rv = getaddrinfo(server_name, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // Loop through results and connect to the first available.
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
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


    pthread_t server_listn;
    pthread_t ui;

    pthread_create(& server_listn,NULL, handle_connection,&sockfd);
    pthread_create(& ui,NULL, get_input,&sockfd);


    pthread_join(server_listn, NULL);
    freeaddrinfo(servinfo); // Free allocated memory



    close(sockfd); // Close the socket.
    return 0;
}