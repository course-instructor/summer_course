#include "common.h"
#include <stdio.h>
#include <string.h>

extern _Atomic enum connection_e g_level;
_Atomic enum request_e g_server_reply; //an atomic enum that is used to check if the server has responded after a request that might change the clients state

/**
 * @brief changes the clients status and prints a message accordingly depending on the servers responce
 * 
 * @param num responce number of the message from the server
 * @param buf the responce itself from the server
 * @return message_s* NULL
 */
message_s * handle_message(int num, const char * buf, void * )
{
    int i = 0;
    switch (num)
    {
        case SIGN_UP_RESPONSE:
            g_server_reply = num;
            if(buf[0] == '0') //success
            {
                printf("signed up sucssesfully\n");
            }

            else//failed (-1)
            {
                printf("couldn't sign up\n");
            }
            break;

        case LOG_IN_RESPONSE:
            g_server_reply = num;
            if(buf[i] == '0') //success
            {
                printf("loged in sucssesfully\n");
                g_level = CONNECTED;
            }

            else//failed (-1)
            {
                printf("couldn't log in\n");
            }
            break;

        case LIST_OF_ROOMS_RESPONSE:
            g_server_reply = num;
            if(buf[i] != '-' && buf[i+1] != '1') //success
            {
                printf("availble rooms: \n");
                char temp [MESSAGE_LENGTH];
                while (!get_param(buf,temp,&i))
                {

                    printf("room: %s\n", temp);

                }
                printf("room: %s\n\n", temp);
            }
            else // failed
            {
                printf("get rooms failed\n");
            }
            break;

        case ENTER_ROOM_RESPONSE:
            g_server_reply = num;
            if(buf[i] == '0') //success
            {
                printf("entered room sucssesfully\n");
                g_level = IN_ROOM;
            }

            else//failed (-1)
            {
                printf("couldnt enter room\n");
            }
            break;

        case MESSAGE_FROM_CLIENT:
            char name [MESSAGE_LENGTH];
            char message[MESSAGE_LENGTH];

            int index = 0;

            get_param(buf, name, &index);
            get_param(buf, message, &index);

            printf("%s: %s\n", name, message);
            break;

        case MESSAGE_FROM_SERVER:
        {
            char message[MESSAGE_LENGTH];

            int index = 0;

            get_param(buf, message, &index);

            printf("server: %s\n" , message);
        }
            break;

        case EXIT_ROOM_RESPONSE:
            g_server_reply = num;
            if(buf[i] == '0') //success
            {
                printf("exited room\n");
                g_level = CONNECTED;
            }

            else//failed (-1)
            {
                printf("didnt exit room\n");
            }
            break;

        default:
            perror("invalid message number\n");
            break;
    }
    return NULL;
}

