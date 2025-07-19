#include  "c_request.h"
#include "common.h"
#include "stdio.h"
#include "client.h"


int send_signup_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.param_count = 2;
    message.request_num = SIGN_UP;

    const char * temp [2] = {name, password};

    message.params = temp;

    return send_message( sockfd, &message);
}

int send_login_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.param_count = 2;
    message.request_num = LOG_IN;

    const char * temp [2] = {name, password};

    message.params = temp;

    return send_message( sockfd, &message);
}


int send_room_lst_message(int sockfd)
{
    message_s message;
    message.param_count = 0;
    message.request_num = LIST_OF_ROOMS;


    message.params = NULL;

    return send_message( sockfd, &message);
}


int send_enter_room_message(int sockfd, const char* name, int room)
{
    message_s message;
    message.param_count = 2;
    message.request_num = ENTER_ROOM;

    char room_str[16];
    snprintf(room_str, sizeof room_str, "%d", room);

    const char * temp [2] = {name, room_str};
    message.params = temp;

    return send_message( sockfd, &message);
}


int send_leave_room_message(int sockfd,const char * name)
{
    message_s message;
    message.param_count = 1;
    message.request_num = EXIT_ROOM;


    const char * temp [1] = {name};
    message.params = temp;

    return send_message( sockfd, &message);
}

int send_in_room_message(int sockfd,const char * name, const char * str)
{
    message_s message;
    message.param_count = 3;
    message.request_num = MESSAGE_ROOM;


    const char * temp [] = {name, str};
    message.params = temp;

    return send_message( sockfd, &message);
}