#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MESSAGE_LENGTH 256

enum connection_e
{
  NOT_CONNECTED,
  CONNECTED,
  IN_ROOM
};

enum request_e
{
    SIGN_UP       = 100,
    LOG_IN,
    LIST_OF_ROOMS,
    ENTER_ROOM,
    MESSAGE_ROOM,
    EXIT_ROOM,

    SIGN_UP_RESPONSE       = 200,
    LOG_IN_RESPONSE,
    LIST_OF_ROOMS_RESPONSE,
    ENTER_ROOM_RESPONSE,
    MESSAGE_FROM_CLIENT,
    MESSAGE_FROM_SERVER,
    EXIT_ROOM_RESPONSE     = 211,
};

typedef struct message_s
{
    enum request_e request_num;
    int param_count;
    const char **params;
} message_s;

typedef struct client_s
{
    int sockfd;
    enum connection_e status;
    struct sockaddr_storage addr;
    int room_index;
} client_s, *client_ptr_t;



static const char* SUCCESS[] __attribute__((unused)) = {"0"}; //success status: success action
static const char* FAIL[] __attribute__((unused)) = {"-1"}; //success status: failed action


message_s * handle_message(int number, const char * buf, void * ptr);

#define SEPERATING_CHAR  '\r'
#define END_CHAR '\n'


int send_signup_message(int sockfd, const char *name, const char *password);
int send_message(int sockfd, const message_s *message);
int get_message(client_ptr_t client);

void *get_in_addr(const struct sockaddr *sa);
int get_param(const char * buf, char * ret, int * index_ptr);
