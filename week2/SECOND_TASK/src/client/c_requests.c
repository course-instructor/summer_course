#include  "c_request.h"
#include "common.h"
#include "stdio.h"
#include "client.h"
#include <stdlib.h>
extern int usleep (__useconds_t __useconds); // unistd.h

extern _Atomic enum request_e g_server_reply;

/**
 * @brief the function makes the client wait after a request that might change the clients state untill the server responds
 *
 * @param msg the responce that the function is waiting for from the server
 */
void wait_for_server_reply(enum request_e msg)
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

/**
 * @brief the function sends signup message to the server
 *
 * @param sockfd server socket
 * @param name the name to sign up
 * @param password the password to sign up
 */
void send_signup_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.request_num = SIGN_UP;

    const char * temp [] = {name, password,NULL};

    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(SIGN_UP_RESPONSE);
}


/**
 * @brief the function sends login message to the server
 *
 * @param sockfd server socket
 * @param name the name to log in
 * @param password the password to log in
 */
void send_login_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.request_num = LOG_IN;

    const char * temp [] = {name, password, NULL};

    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(LOG_IN_RESPONSE);
}

/**
 * @brief  the function sends a room list message
 *
 * @param sockfd the servers socket
 */
void send_room_lst_message(int sockfd)
{
    message_s message;
    message.request_num = LIST_OF_ROOMS;


    message.params = NULL;

    send_message( sockfd, &message);
    wait_for_server_reply(LIST_OF_ROOMS_RESPONSE);
}

/**
 * @brief  the function sends a enter room request
 *
 * @param sockfd servers socket
 * @param name name to enter as
 * @param room room to enter
 */
void send_enter_room_message(int sockfd, const char* name, int room)
{
    message_s message;
    message.request_num = ENTER_ROOM;

    char room_str[16];
    snprintf(room_str, sizeof room_str, "%d", room);

    const char * temp [] = {name, room_str, NULL};
    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(ENTER_ROOM_RESPONSE);
}

/**
 * @brief  the function sends leave room message to the server
 *
 * @param sockfd servers socket
 * @param name name to exit as
 */
void send_leave_room_message(int sockfd,const char * name)
{
    message_s message;
    message.request_num = EXIT_ROOM;


    const char * temp [] = {name, NULL};
    message.params = temp;

    send_message( sockfd, &message);
    wait_for_server_reply(EXIT_ROOM_RESPONSE);
}

/**
 * @brief  the function sends a message to the server to send to all the other clients in the room
 *
 * @param sockfd servers socket
 * @param name the name that the message will be from
 * @param str the message that will be sent to everyone in the room
 */
void send_in_room_message(int sockfd,const char * name, const char * str)
{
    message_s message;
    message.request_num = MESSAGE_ROOM;


    const char * temp [] = {name, str, NULL};
    message.params = temp;

    send_message( sockfd, &message);

}