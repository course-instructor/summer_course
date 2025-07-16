#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "common.h"


static int send_message(const message_s * message, int sockfd)
{
    int param_index = 0;
    char payload [MESSAGE_LENGTH];
    snprintf(payload, MESSAGE_LENGTH, "%d%c%s", payload, SEPERATING_CHAR, message->request_num);
    while(param_index < (message->param_count))
    {
        snprintf(payload, MESSAGE_LENGTH, "%s%c%s", payload,SEPERATING_CHAR, message->params[param_index]);
    }

    snprintf(payload, MESSAGE_LENGTH, "%s%c", payload, END_CHAR);

    return send(sockfd, payload,sizeof(payload), 0);
}

void read_message(char * buf)
{
    int number;
    
    
}

int send_signup_message(int sockfd, const char *name, const char *password) 
{
    message_s *message = (struct message_s *)malloc(sizeof(message_s));
    message ->param_count = 2;
    message->request_num = SIGN_UP;

    char * temp [2] = {name, password};

    message ->params = temp;

    return send_message(message, sockfd);
}

int send_login_message(int sockfd, const char *name, const char *password) 
{
    int message_length = sizeof(struct message_s) + strlen(name) + 1 + strlen(password) + 1;
    struct message_s *message = (struct message_s *)malloc(message_length);
    message->length = message_length;
    message->request_num = LOG_IN;
    strcpy(message->payload, name);
    strcpy(message->payload + strlen(name) + 1, password);
    return send_message(message, sockfd);
}

int send_room_lst_message(int sockfd)
{
    int message_length = sizeof(struct message_s) ;
    struct message_s *message = (struct message_s *)malloc(message_length);
    message->length = message_length;
    message->request_num = LIST_OF_ROOMS;
    return send_message(message, sockfd);
}
int send_enter_room(int sockfd, int room_num, char * name)
{
    int message_length = sizeof(struct message_s) + sizeof(room_num) + 1 + sizeof(name) + 1 ;
    struct message_s *message = (struct message_s *)malloc(message_length);
    message->length = message_length;
    message->request_num = ENTER_ROOM;
    strcpy(message->payload, room_num);
    strcpy(message->payload, '\0');

    strcpy(message->payload, name);

    return send_message(message, sockfd);
}
