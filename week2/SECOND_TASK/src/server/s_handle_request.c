#include <stdio.h>    // for FILE, fopen, fwrite, fclose, printf
#include <string.h>   // for strcmp, strcpy, strlen
#include <stdlib.h>   // for malloc, free, exit

#include "s_handle_request.h"
#include "common.h"
#include "s_rooms.h"  // for client_ptr_t and room APIs


extern room_s g_rooms [ROOM_COUNT];

/**
 * @brief the function reads the content of the request message and signs up client in if he is successfull
 * 
 * @param buf the message string striped of the request number that contains the name and password
 * @return message_s* a message to send to the user "-1" fail, "0" success 
 */
message_s * handle_signup(const char * buf)
{
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

/**
 * @brief the function reads the content of the request message and logs client in if he is successfull
 * 
 * @param buf message with name and password
 * @param client client that wants to log in (his status will be changed if he loges in successfully)
 * @return message_s* the responce message "-1" fail, "0" success 
 */
message_s * handle_login(const char * buf, client_s * client)
{
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

/**
 * @brief the function reads the content of the request message and adds him to room in if he is successfull
 * 
 * @param buf the message with the name and room num to try and enter
 * @param client client that wants to enter room 
 * @return message_s* the responce message "-1" fail, "0" success 
 */
message_s * handle_enter_room(const char * buf, client_s * client)
{
    message_s * message = malloc(sizeof(message_s));
    message->request_num = ENTER_ROOM_RESPONSE;
    message->param_count = 1;

    int reading_index = 0;

    char name[MESSAGE_LENGTH];
    get_param(buf,name, &reading_index);



    if((client -> status) == CONNECTED)
    {
        int reading_index = 0;

        char name[MESSAGE_LENGTH];
        get_param(buf,name, &reading_index);

        int room_num;
        char room_str[MESSAGE_LENGTH];

        get_param(buf, room_str, &reading_index);

        sscanf(room_str, "%d", &room_num);

        int success = process_enter_room(client, room_num);



        if(success)
        {
            // message_s * broadcast_message = create_message("has entered room",name);
            broadcast(& g_rooms[client->room_index], client, name, "has entered room");
            // free(broadcast_message);
            message->params = SUCCESS;
        }

        else
        {
            message->params = FAIL;
        }
    }
    return message;
}

/**
 * @brief the function reads the content of the request message and removes him from room if he is successfull
 * 
 * @param buf the message with the name
 * @param client client that wants to leave room 
 * @return message_s*  the responce message "-1" fail, "0" success 
 */
message_s * handle_exit_room(const char * buf,client_s * client)
{
    message_s * message = malloc(sizeof(message_s));
    message->request_num = EXIT_ROOM_RESPONSE;
    message->param_count = 1;

    int reading_index = 0;

    char name[MESSAGE_LENGTH];
    get_param(buf,name, &reading_index);

    int success = proccess_exit_room(client );

    if(success)
    {
        // message_s * broadcast_message = create_message("has left room",name);
        broadcast(& g_rooms[client->room_index], client, name, "has left room");
        // free(broadcast_message);
        message->params = SUCCESS;
    }
    else
    {
        message->params = FAIL;
    }
    return message;
}

/**
 * @brief  the function reads the content of the request message and sends message in room under the name 
 * 
 * @param buf the message with the name and message to send to room
 * @param client client that wants to seend to room a message
 * @return message_s* NULL
 */
message_s * handle_send_to_room(const char *buf, client_s * client)
{
    // message_s * message = malloc(sizeof(message_s));
    // message->request_num = MESSAGE_FROM_CLIENT;
    // message->param_count = 2;

    int reading_index = 0;

    char name[MESSAGE_LENGTH];
    get_param(buf,name, &reading_index);

    char str[MESSAGE_LENGTH];
    get_param(buf, str, &reading_index);

    // const char * temp [2] = {name,str};

    // message->params = temp;

    broadcast(& g_rooms[client->room_index] , client, name, str);

    return NULL;
}

/**
 * @brief the function calls the apropriete request handler for each request and returns the message it gets from the handler
 * 
 * @param num request number
 * @param buf the rest of the message (without request number)
 * @param ptr client pointer 
 * @return message_s* message that it got from handler
 */
message_s * handle_message(int num, const char * buf, void * ptr)
{
    client_s * client = (client_s *) ptr;
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
        case MESSAGE_ROOM:
            response_message = handle_send_to_room(buf, client);
            break;
        case EXIT_ROOM:
            response_message = handle_exit_room(buf,client);
            break;
        default:
            printf("unknown message %d\n", num);
            break;
    }
    return response_message;
}


/**
 * @brief the function tries to signup the client (succes if the client doesnt already exist)
 * 
 * @param name the username of the client
 * @param password the password of the client
 * @return int 1 succesfull signup, 0 didnt succeed
 */
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
/**
 * @brief the function tries to find the client and checks if the password is correct
 * 
 * @param name clients name
 * @param password attempted password
 * @return int was the client found and the password mach?
 */
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
                login_error = strcmp(line, password);//login unsuccessfull when passwords dont match...
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

/**
 * @brief the function creates a message with the names of the available room
 * 
 * @param client client that wants to get the list of rooms
 * @return message_s* list of rooms or {-1} if unssuccessfull
 */
message_s * handle_room_lst_message(client_s * client)
{
    message_s * message = NULL;
    if((client -> status) == CONNECTED)
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

/**
 * @brief the function tries to add client to room
 * 
 * @param client client to add to the room
 * @param room_num room that the client wants to enter
 * @return int did the client enter the room
 */
int process_enter_room(client_ptr_t client,int room_num)
{

    int enter_room_error = 0;
    if((client -> status) == CONNECTED && room_num < ROOM_COUNT)
    {
        client->room_index = room_num;
        client -> status = IN_ROOM;
        room_add_client(& g_rooms[room_num], client);
    }
    return !enter_room_error;
}

/**
 * @brief the function tries to remove client from room
 * 
 * @param client client that wants to leave the room
 * @return int did the client leave the room
 */
int proccess_exit_room(client_s * client)
{
    int enter_room_error = 0;
    if((client -> status) == IN_ROOM)
    {
        //send message in room

        client -> status = CONNECTED;

        room_rem_client(& g_rooms[client->room_index], client);

    }
    return !enter_room_error;
}