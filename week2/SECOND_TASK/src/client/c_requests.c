#include  "c_request.h"
#include "common.h"
#include "stdio.h"


int send_signup_message(int sockfd, const char *name, const char *password)
{
    message_s message;
    message.param_count = 2;
    message.request_num = SIGN_UP;

    const char * temp [2] = {name, password};

    message.params = temp;

    return send_message( sockfd, &message);
}

// int send_login_message(int sockfd, const char *name, const char *password)
// {
//     int message_length = sizeof(struct message_s) + strlen(name) + 1 + strlen(password) + 1;
//     struct message_s *message = (struct message_s *)malloc(message_length);
//     message->length = message_length;
//     message->request_num = LOG_IN;
//     strcpy(message->payload, name);
//     strcpy(message->payload + strlen(name) + 1, password);
//     return send_message( sockfd, message);
// }

// int send_room_lst_message(int sockfd)
// {
//     int message_length = sizeof(struct message_s) ;
//     struct message_s *message = (struct message_s *)malloc(message_length);
//     message->length = message_length;
//     message->request_num = LIST_OF_ROOMS;
//     return send_message( sockfd, message);
// }
// int send_enter_room(int sockfd, int room_num, char * name)
// {
//     int message_length = sizeof(struct message_s) + sizeof(room_num) + 1 + sizeof(name) + 1 ;
//     struct message_s *message = (struct message_s *)malloc(message_length);
//     message->length = message_length;
//     message->request_num = ENTER_ROOM;
//     strcpy(message->payload, room_num);
//     strcpy(message->payload, '\0');

//     strcpy(message->payload, name);

//     return send_message( sockfd, message);
// }
