#include  "c_request.h"
#include "common.h"
#include "stdio.h"
#include "client.h"
#include <stdlib.h>
extern int usleep (__useconds_t __useconds); // unistd.h

extern _Atomic enum request_e g_server_reply;
void wait_for_server_reply(enum request_e msg)
{
    printf("waiting for %d\n", msg);
    g_server_reply = ZERO;
    while (g_server_reply == ZERO)
    {
        usleep(10000); // 10 ms
    }
    if (g_server_reply != msg)
    {
        printf("unexpected server reply %d, exit\n", g_server_reply);
        exit(1);
    }
}

void send_signup_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.param_count = 2;
    message.request_num = SIGN_UP;

    const char * temp [2] = {name, password};

    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(SIGN_UP_RESPONSE);
}

void send_login_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.param_count = 2;
    message.request_num = LOG_IN;

    const char * temp [2] = {name, password};

    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(LOG_IN_RESPONSE);
}


void send_room_lst_message(int sockfd)
{
    message_s message;
    message.param_count = 0;
    message.request_num = LIST_OF_ROOMS;


    message.params = NULL;

    send_message( sockfd, &message);
    wait_for_server_reply(LIST_OF_ROOMS_RESPONSE);
}


void send_enter_room_message(int sockfd, const char* name, int room)
{
    message_s message;
    message.param_count = 2;
    message.request_num = ENTER_ROOM;

    char room_str[16];
    snprintf(room_str, sizeof room_str, "%d", room);

    const char * temp [2] = {name, room_str};
    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(ENTER_ROOM_RESPONSE);
}


void send_leave_room_message(int sockfd,const char * name)
{
    message_s message;
    message.param_count = 1;
    message.request_num = EXIT_ROOM;


    const char * temp [1] = {name};
    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(EXIT_ROOM_RESPONSE);
}

void send_in_room_message(int sockfd,const char * name, const char * str)
{
    message_s message;
    message.param_count = 2;
    message.request_num = MESSAGE_ROOM;


    const char * temp [] = {name, str};
    message.params = temp;

    send_message( sockfd, &message);

}