#include "common.h"

#define MESSAGE_LENGTH 256
#include <string.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include "common.h"


extern void handle_message(char * buf, int num);



int send_message(const message_s * message, int sockfd)
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


int get_message(int sockfd)
{
    int numbytes;
    char buf[MESSAGE_LENGTH];


    int number = 0;

    if ((numbytes = recv(sockfd, buf, MESSAGE_LENGTH, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }


    while ((*buf) ++ != SEPERATING_CHAR)
    {
        number *= 10;
        number += (*buf) - '0';
    }

    handle_message(buf, number);
}
void *get_in_addr( struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


int get_param(char * buf, char * ret)
{
    int end_flag = 0;
    ret = buf;

    int length = strchr(buf,SEPERATING_CHAR);
    strncpy(ret,buf, length);
    buf += length + 1;
    if(*(buf) == END_CHAR)
    {
        *(buf) = '\0';
        end_flag = 1;
    }
    
    return(end_flag);
}