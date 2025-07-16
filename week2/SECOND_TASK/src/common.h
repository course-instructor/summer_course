#pragma once

#define MESSAGE_LENGTH 256

enum request_e
{
    SIGN_UP = 100,
    LOG_IN ,
    LIST_OF_ROOMS,
    ENTER_ROOM,
    MESSEGE_ROOM,
    EXIT_ROOM,
};

enum response_e
{
    SIGN_UP_RESPONSE = 200,
    LOG_IN_RESPONSE ,
    LIST_OF_ROOMS_RESPONSE,
    ENTER_ROOM_RESPONSE,
    MESSAGE_FROM_CLIENT,
    MESSAGE_FROM_SERVER,
    EXIT_ROOM_RESPONSE = 211,
};

typedef struct message_s
{
    int param_count;
    enum request_e request_num;
    char ** params;
}message_s;


#define SEPERATING_CHAR '\r'
#define END_CHAR '\n'

int send_signup_message(int sockfd, const char *name, const char *password);


