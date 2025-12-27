#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h> // NULL

#define MESSAGE_LENGTH 256 //max length of the message
#define PORT "3490" 

typedef enum
{
    FALSE,
    TRUE
}bool_e

/// @brief the connection access status of the client
typedef enum connection_e
{
  NOT_CONNECTED,
  CONNECTED,
  IN_ROOM
}connection_e;

/// @brief request codes that the user sends to the server
typedef enum request_e
{
    SIGN_UP       = 100,
    LOG_IN,
    LIST_OF_ROOMS,
    ENTER_ROOM,
    MESSAGE_ROOM,
    EXIT_ROOM,


}request_e;
typedef enum responce_e
{
    SIGN_UP_RESPONSE       = 200,
    LOG_IN_RESPONSE,
    LIST_OF_ROOMS_RESPONSE,
    ENTER_ROOM_RESPONSE,
    MESSAGE_FROM_CLIENT,
    MESSAGE_FROM_SERVER,
    EXIT_ROOM_RESPONSE     = 211,

    WAITING = 0
}responce_e;

typedef union
{
    enum request_e req;
    enum responce_e res;
} message_code_u;


/// @brief message struct that will be reformated into a string before sending
typedef struct message_s
{
    message_code_u message_num; //request number of the message(what tipe of message)
    int is_response_msg; //0 for req, 1 for res
    const char **params; // a NULL terminated array of params to be passed
} message_s;

/// @brief client struct that is used to store different info about the client
typedef struct client_t
{
    int sockfd; // the clients socket
    enum connection_e status; // the connections status ( what the client is allowed to do)
    struct sockaddr_storage addr; //sock address
    int room_index; //room_index
} client_t, *client_ptr_t;



static const char* SUCCESS[] __attribute__((unused)) = {"0", NULL}; //success status: success action
static const char* FAIL[] __attribute__((unused)) = {"-1", NULL}; //success status: failed action


message_s * handle_message(int number, const char * buf, void * ptr);

#define SEPERATING_CHAR  '\r' //char after every message param
#define END_CHAR '\n' //char at the end of the message


int send_message(int sockfd, const message_s *message);
int get_message(client_ptr_t client);

void *get_in_addr(const struct sockaddr *sa);
int get_param(const char * buf, char * ret, int * index_ptr);
