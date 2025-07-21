#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

/**
 * @brief sends message to a socket: message_num param1 param2 ... after each param seperating char, at the end end_char
 *
 * @param sockfd socket to send to
 * @param message mesage_s pointer (to write the params and request number to the socket)
 * @return int send success
 */
int send_message(int sockfd, const message_s *message)
{
    char payload[MESSAGE_LENGTH];

    int number = (message->code_tag)?(message->message_num.res):(message->message_num.res);

    int  pos = snprintf(payload, sizeof payload, "%d%c",number ,SEPERATING_CHAR);

    if(message->params)
    {
        for (int i = 0; message->params[i]; i++) //loop until NULL params terminator
        {
            pos += snprintf(payload + pos, sizeof payload - pos, "%s%c",  message->params[i], SEPERATING_CHAR);
        }
    }

    payload[pos++] = END_CHAR;

    return send(sockfd, payload, pos, 0);
}
/**
 * @brief recv a message from the socket stored in client read the request number then call the handle_message function (unique for client, server)
 *
 * @param client sent to handle_message, is used to get the socket to listen to...
 * @return int 1 if connected else 0
 */
int get_message(client_ptr_t client)
{
    int sockfd = client->sockfd;

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
        buf_ptr++;
        message_s * response = handle_message(number, buf_ptr, client);

        if (response)
        {
            send_message(sockfd, response);
            free(response);
        }

    }
    return is_connected;
}

/**
 * @brief Get the in addr
 *
 * @param sa sock adders that we read the address from
 * @return void* ipv4 or ipv6
 */
void *get_in_addr(const struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    else
    {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

/**
 * @brief Get the next param from the buffer and increment the reading index to the current reading index
 *
 * @param buf string that the param is read from
 * @param ret the string that the param is writen to (and null terminated)
 * @param index_ptr reading index pointer the function reads a parameter from the string at this index and increments it to the index of the start of the next param
 * @return int whether this param was the last in the string (after the seperating char there was the end char)
 */
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
