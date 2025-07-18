#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


int send_message(int sockfd, const message_s *message)
{
    char payload[MESSAGE_LENGTH];
    int  pos = snprintf(payload, sizeof payload, "%d%c",message->request_num,SEPERATING_CHAR);

    for (int i = 0; i < message->param_count; i++)
    {
        pos += snprintf(payload + pos, sizeof payload - pos, "%s%c",  message->params[i], SEPERATING_CHAR);
    }
    payload[pos++] = END_CHAR;
    return send(sockfd, payload, pos, 0);
}

int get_message(int sockfd)
{
    printf("get message\n");

    int numbytes;
    char buf[MESSAGE_LENGTH];
    char * buf_ptr = buf;

    int is_connected = 1;


    numbytes = recv(sockfd, buf, MESSAGE_LENGTH, 0);
    if (numbytes <= 0)
    {
        is_connected = 0;
    }
    else
    {
        int number = 0;

        while (*(buf_ptr ) != SEPERATING_CHAR)
        {
            number *= 10;
            number += (*buf_ptr) - '0';
            buf_ptr ++;
        }
        printf("%d\n",number);
        buf_ptr++;
        message_s * response = handle_message(number, buf_ptr);
        if (response)
        {
            send_message(sockfd, response);
            free(response);
        }

    }
    return is_connected;
}

void *get_in_addr(const struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_param(const char * buf, char * ret, int * index_ptr)
{
    int end_flag = 0;
    int i=0;

    while(buf[*index_ptr] != SEPERATING_CHAR)
    {
        ret[i++] = buf[*index_ptr];
        (*index_ptr)++;
    }

    ret[i] = '\0';
    (*index_ptr) ++;
    if(buf[*index_ptr] == END_CHAR)
    {
        end_flag = 1;
    }

    return end_flag;
}
