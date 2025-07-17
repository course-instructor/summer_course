#include <stdio.h>    // for FILE, fopen, fwrite, fclose, printf
#include <string.h>   // for strcmp, strcpy, strlen
#include <stdlib.h>   // for malloc, free, exit

#include "s_handle_request.h"
#include "common.h"
#include "s_rooms.h"  // for client_ptr_t and room APIs



#include "common.h"
#include "s_handle_request.h"

extern room_s g_rooms [ROOM_COUNT];

void handle_message(char * buf, int num)
{
    switch (num)
    {
    case SIGN_UP:
        char name [MESSAGE_LENGTH];
        get_param(buf,name);
        char pass [MESSAGE_LENGTH];
        get_param(buf,pass);

        int succes = handle_signup(name,pass);
        break;
    
    default:
        break;
    }
}



int handle_signup(const char *name, const char *password) 
{
    FILE *f = fopen("users.txt", "r");
    if (!f) 
    {
        f = fopen("users.txt", "a");
        if (!f) return -1;
    } else 
    {
        char line[MESSAGE_LENGTH];
        while (fgets(line, sizeof line, f)) 
        {
            if (strcmp(line, name) == 0) // already exists
            {
                fclose(f);
                return -1;  
            }
            if (!fgets(line, sizeof line, f)) break;
        }
        fclose(f);
        f = fopen("users.txt", "a");
        if (!f) 
        {
            return -1;
        }
    }

    // Append new user: name\npassword\n
    fprintf(f, "%s\n%s\n", name, password);
    fclose(f);
    return 0;
}

int handle_login(client_s c,const char *name, const char *password)
{
    FILE * f = fopen("users.txt", 'r');
    
    char * str;
    char * name_r;
    *name_r = '\0';

    while(( fgets(str,MESSAGE_LENGTH,f)) && (c . status == NOT_CONNECTED))
    {
        if(str == '\n')
        {

        }
        else
        {
            if (strcmp(str,name) == 0)
            {
                strcpy(name_r,str);
            }
            str = fgets(str, f , MESSAGE_LENGTH);
                
            if(name_r && strcmp(str,password) == 0)
            {
                c . name = name;
                c . status = CONNECTED;
            }
        }
    }
    return(c . status );
}


// int handle_room_lst_message(client_s * c)
// {
//     char * msg;
//     if((c -> status) != NOT_CONNECTED)
//     {
//         for(int i = 0; i < ROOM_COUNT; i++)
//         {
//             strcat(msg,i);
//             strcat(msg, '\0');
//         }
//         int message_length = sizeof(struct message_s);
//         struct message_s *message = (struct message_s *)malloc(message_length);
//         message->length = message_length;
//         message->request_num = LIST_OF_ROOMS;
//         return send_message(message, sockfd);
//     }
// }


// int handle_enter_room(client_s c, int room_num)
// {
//     if((c -> status) != NOT_CONNECTED)
//     {
//         if(room_num <  ROOM_COUNT && room_num > 0)
//         {
//             c -> room = g_rooms[i];
//             c -> status =   IN_ROOM;

//             client_s top = g_rooms[i] -> clients;
//             mutex_lock (g_rooms[i]);  
//             while
//         }

//         int message_length = sizeof(struct message_s);
//         struct message_s *message = (struct message_s *)malloc(message_length);
//         message->param_count = 2;
//         message->params = g_rooms;
//         message->request_num = LIST_OF_ROOMS;
//         return send_message(message, sockfd);
//     }
// }
