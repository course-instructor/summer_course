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
    response_message->request_num = LOG_IN_RESPONSE;

    if(success)
    {
        printf("logged in successfully\n");
        response_message->params = SUCCESS;
        client->status = CONNECTED;
    }
    else
    {
        printf("could not log in\n");
        response_message->params = FAIL;
    }
    return response_message;
}

message_s * handle_enter_room(const char * buf, client_s * c)
{
    message_s * message = malloc(sizeof(message_s));
    message->request_num = ENTER_ROOM_RESPONSE;
    message->param_count = 1;

    int reading_index = 0;

    char name[MESSAGE_LENGTH];
    get_param(buf,name, &reading_index);



    if((c -> status) == CONNECTED)
    {
        int reading_index = 0;

        char name[MESSAGE_LENGTH];
        get_param(buf,name, &reading_index);

        int room_num;
        char room_str[MESSAGE_LENGTH];

        get_param(buf, room_str, &reading_index);

        sscanf(room_str, "%d", &room_num);

        int success = process_enter_room(c, room_num);

        if(success)
        {
            //send message to all clients: 'client {name} has entered the room'

            message->params = SUCCESS;
        }

        else
        {
            message->params = FAIL;
        }
    }
    return message;
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
        case LIST_OF_ROOMS:
            response_message = handle_room_lst_message(client);
            break;
        case ENTER_ROOM:
            response_message = handle_enter_room(buf, client);
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
                fgets(line, sizeof line, f);
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


message_s * handle_room_lst_message(client_s * c)
{
    message_s * message = NULL;
    if((c -> status) == CONNECTED)
    {
        message = malloc(sizeof(message_s));
        message->request_num = LIST_OF_ROOMS_RESPONSE;
        message ->param_count = ROOM_COUNT;
        message->params = malloc(ROOM_COUNT * 20);
        for(int i = 0; i < ROOM_COUNT; i++)
        {
            message->params[i] = g_rooms[i].name;
        }

    }
    return message;
}


int process_enter_room(client_ptr_t c,int room_num)
{
    int enter_room_error = 0;
    if((c -> status) == CONNECTED && room_num < ROOM_COUNT)
    {
        c->room_index = room_num;
        c -> status = IN_ROOM;

        pthread_mutex_lock(& g_rooms[c->room_index].mutex);
        room_s room = g_rooms[c->room_index];
        add_client(room.clients, c);
        pthread_mutex_unlock(& room.mutex);
    }
    return !enter_room_error;
}
