#include "common.h"
#include <stdio.h>
#include <string.h>

extern enum connection_e g_level;

message_s * handle_message(int num, const char * buf, void * )
{
    int i = 0;
    switch (num)
    {
        case (SIGN_UP_RESPONSE):
            if(buf[0] == '0') //success
            {
                printf("signed up sucssesfully\n");
            }

            else//failed (-1)
            {
                printf("couldn't sign up\n");
            }
            break;

        case (LOG_IN_RESPONSE):
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

        case (LIST_OF_ROOMS_RESPONSE):
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

        // case (MESSAGE_FROM_CLIENT):
        //     char * name;
        //     char * message;

        //     get_param(buf, name);
        //     get_param(buf, message);

        //     printf("%s: %s\n", name, message);
        //     break;

        // case (MESSAGE_FROM_SERVER):
        //     printf("server: %s\n" , buf);
        //     break;

        // case (EXIT_ROOM_RESPONSE):
        //     if(buf[i] == '0') //success
        //     {
        //         printf("logged in sucssesfully\n");
        //         g_level = LOG_IN;
        //     }

        //     else//failed (-1)
        //     {
        //         printf("logged in un-sucssesfully\n");
        //     }
        //     break;

        default:
            perror("invalid message number\n");
            break;
    }
    return NULL;
}

