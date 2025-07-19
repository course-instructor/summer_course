#include <stdio.h>    // for FILE, fopen, fwrite, fclose, printf
#include <string.h>   // for strcmp, strcpy, strlen
#include <stdlib.h>   // for malloc, free, exit

#include "s_handle_request.h"
#include "common.h"
#include "s_rooms.h"  // for client_ptr_t and room APIs


extern room_s g_rooms [ROOM_COUNT];

message_s * handle_signup(const char * buf)
{
    printf("recived signup\n");
    int reading_index = 0;

    char name [MESSAGE_LENGTH];
    get_param(buf,name, &reading_index);
    printf("name: %s\n", name);
    char pass [MESSAGE_LENGTH];
    get_param(buf,pass, &reading_index);
    printf("pass: %s\n", pass);

    int success = process_signup(name,pass);

    message_s * response_message = (message_s *)malloc(sizeof (struct message_s));
    response_message->param_count = 1;
    response_message->request_num = SIGN_UP_RESPONSE;

    if(success)
    {
        printf("signed up successfully\n");
        response_message->params = SUCCESS;
    }
    else
    {
        printf("could not sign up\n");
        response_message->params = FAIL;
    }
    return response_message;
}

message_s * handle_login(const char * buf, client_ptr_t client)
{
    printf("recived login\n");
    int reading_index = 0;

    char name [MESSAGE_LENGTH];
    get_param(buf,name, &reading_index);
    printf("name: %s\n", name);
    char pass [MESSAGE_LENGTH];
    get_param(buf,pass, &reading_index);
    printf("pass: %s\n", pass);

    int success = process_login(name,pass);

    message_s * response_message = (message_s *)malloc(sizeof (struct message_s));
    response_message->param_count = 1;
    response_message->request_num = SIGN_UP_RESPONSE;

    if(success)
    {
        printf("logged in successfully\n");
        response_message->params = SUCCESS;
        client ->status = CONNECTED;
        strcpy(client->name,name);
    }
    else
    {
        printf("could not log in\n");
        response_message->params = FAIL;
    }
    return response_message;
}



message_s * handle_message(int num, const char * buf, client_ptr_t client)
{
    message_s * response_message = NULL;

    switch (num)
    {
        case SIGN_UP:
            response_message = handle_signup(buf);
            break;
        case LOG_IN:
            response_message = handle_login(buf, client);
            break;
        case LIST_OF_ROOMS: // on cient
            break;
        default:
            printf("unknown message %d\n", num);
            break;
    }
    return response_message;
}



int process_signup(const char *name, const char *password)
{
    int signup_error = 0;
    FILE *f = fopen("users.txt", "r");
    if (f)
    {
        char line[MESSAGE_LENGTH];
        while (fgets(line, sizeof line, f))
        {
            *(strchr(line, '\n')) = '\0';
            if (strcmp(line, name) == 0) // already exists
            {
                printf("user: %s already exists\n", name);
                signup_error = 1;
            }
            if (!fgets(line, sizeof line, f)) //skip password
            {
                break;
            }
        }
        fclose(f);
    }

    if(!signup_error)
    {
        f = fopen("users.txt", "a");
        if (!f)
        {
            perror("cannot open users.txt for append");
            signup_error = 1; // didnt register so return 0
        }
        else
        {
            // Append new user: name\npassword\n
            fprintf(f, "%s\n%s\n", name, password);
            fclose(f);
        }
    }

    return !signup_error;
}

int process_login(const char *name, const char *password)
{
    int login_error = 1;
    FILE * f = fopen("users.txt", "r");
    if(f)

    {
        char line[MESSAGE_LENGTH];

        while (fgets(line, sizeof line, f))
        {
            *(strchr(line, '\n')) = '\0';
            if (strcmp(line, name) == 0) //found client
            {
                *(strchr(line, '\n')) = '\0';
                login_error = strcmp(line, password) == 0;//login unsuccessfull when passwords dont match...
                break; //name is unique for each user...
            }
            if (!fgets(line, sizeof line, f)) //skip password if name wasnt found (and exit loop if its the last)
            {
                break;
            }
        }
        fclose(f);
    }
    return(! login_error );
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
//         return send_message( sockfd, message);
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
//         return send_message( sockfd, message);
//     }
// }
