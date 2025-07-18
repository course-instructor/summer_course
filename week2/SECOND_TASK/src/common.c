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
    printf("%d \n",pos);

    for (int i = 0; i < message->param_count; i++)
    {
        pos += snprintf(payload + pos, sizeof payload - pos, "%s%c",  message->params[i], SEPERATING_CHAR);
        printf("%d \n",pos);
    }
    payload[pos++] = END_CHAR;
    printf("%s\n\n", payload);
    return send(sockfd, payload, pos, 0);
}

int get_message(int sockfd)
{
    printf("getting char\n");

    int numbytes;
    char buf[MESSAGE_LENGTH];
    char * buf_ptr = buf;

    int is_connected = 1;


    int number = 0;

    if ((numbytes = recv(sockfd, buf, MESSAGE_LENGTH, 0)) == -1)
    {
        printf("connection lost");
        is_connected = 0;
    }
    else
    {

        while (*(buf_ptr ) != SEPERATING_CHAR)
        {
            number *= 10;
            number += (*buf_ptr) - '0';
            buf_ptr ++;
        }
        printf("%d\n",number);
        buf_ptr++;
        is_connected = handle_message(buf_ptr, number);
    }
    return is_connected;
}

void *get_in_addr(const struct sockaddr *sa) {
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);
    else
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_param(const char * buf, char * ret)
{
    int end_flag = 0;
    char * seperting = strchr(END_CHAR);

    strncpy(ret, buf, )

    strcpy(ret,buf)

    int length = strchr(buf,SEPERATING_CHAR);

    buf += length + 1;
    if(*(buf) == END_CHAR)
    {
        *(buf) = '\0';
        end_flag = 1;
    }

    return end_flag;
}
