#include  "c_request.h"
#include "common.h"
#include "stdio.h"
#include "client.h"
#include <stdlib.h>
extern int usleep (__useconds_t __useconds); // unistd.h

extern _Atomic enum responce_e g_server_reply;

/**
 * @brief the function makes the client wait after a request that might change the clients state untill the server responds
 *
 * @param msg the responce that the function is waiting for from the server
 */
void wait_for_server_reply(enum responce_e msg)
{
    printf("waiting for %d\n", msg);
    g_server_reply = WAITING;
    while (g_server_reply == WAITING)
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
    message.message_num.req = SIGN_UP;
    message.is_response_msg = 0; //req

    const char * temp [] = {name, password,NULL};

    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(SIGN_UP_RESPONSE);
}


void send_login_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.message_num.req = LOG_IN;
    message.is_response_msg = 0; //req


    const char * temp [] = {name, password, NULL};

    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(LOG_IN_RESPONSE);
}

void send_room_lst_message(int sockfd)
{
    message_s message;
    message.message_num.req = LIST_OF_ROOMS;
    message.is_response_msg = 0; //req


    message.params = NULL;

    send_message( sockfd, &message);
    wait_for_server_reply(LIST_OF_ROOMS_RESPONSE);
}

void send_enter_room_message(int sockfd, const char* name, int room)
{
    message_s message;
    message.message_num.req = ENTER_ROOM;
    message.is_response_msg = 0; //req


    char room_str[16];
    snprintf(room_str, sizeof room_str, "%d", room);

    const char * temp [] = {name, room_str, NULL};
    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(ENTER_ROOM_RESPONSE);
}

void send_leave_room_message(int sockfd,const char * name)
{
    message_s message;
    message.message_num.req = EXIT_ROOM;
    message.is_response_msg = 0; //req


    const char * temp [] = {name, NULL};
    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(EXIT_ROOM_RESPONSE);
}


void send_in_room_message(int sockfd,const char * name, const char * str)
{
    message_s message;
    message.message_num.req = MESSAGE_ROOM;
    message.is_response_msg = 0; //req


    const char * temp [] = {name, str, NULL};
    message.params = temp;

    send_message( sockfd, &message);

}