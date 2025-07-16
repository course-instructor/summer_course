#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define ROOM_COUNT 5
enum connection_status {NOT_CONNECTED, CONNECTED, IN_ROOM};

typsef struct 
{
    int sockfd;
    enum connection_status status;
    char * name;
    int room_num = -1;
    room_s room;
} * client_s;

typdef struct 
{
    client  top;
    client * next;
} * node;


typdef struct 
{
    node clients;
} * room_s;

node g_clients = malloc(sizeof(node));
g_clients -> top = NULL;
g_clients -> next = NULL;


room [ROOM_COUNT] g_rooms ;
for(int i = 0; i < ROOM_COUNT; i++)
{
    rooms[i] = malloc(sizeof(room));
    rooms[i] -> clients = NULL;
}


int handle_signup(const char *name, const char *password)
{
    FILE * f = fopen("users.txt", 'r');
    char * str;
    int exists_flag = 0;
    while(f && str = fgets(f) && !exists_flag)
    {
        if(strcmp(str, name) == 0)
        {
            exists_flag = 1;
        }
        fgets(f);
        fgets(f);
    }


    if(!exists_flag)
    {
        FILE * f = fopen("users.txt", 'a');
        fwrite(f, name);
        fwrite(f, '\0');
        fwrite(f, password);
        fwrite(f, '\0\n\0')
        fclose(f);
    }
}

int handle_login(client c,const char *name, const char *password)
{
    FILE * f = fopen("users.txt", 'r');
    
    char * str;
    char * name_r;
    *name_r = '\0';

    while((str = fgets(f)) && (C -> connected_status == NOT_CONNECTED))
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
            str = fgets(f);
                
            if(name_r && strcmp(str,password) == 0)
            {
                c -> name = name;
                c -> connected_status = CONNECTED;
            }
        }
    }
    return(c -> connected_status);
}


int handle_room_lst_message(client c)
{
    char * msg;
    if((c -> status) != NOT_CONNECTED)
    {
        for(int i = 0; i < ROOM_COUNT; i++)
        {
            strcat(msg,i);
            strcat(msg, '\0');
        }
        int message_length = sizeof(struct message_s);
        struct message_s *message = (struct message_s *)malloc(message_length);
        message->length = message_length;
        message->request_num = LIST_OF_ROOMS;
        return send_message(message, sockfd);
    }
}


int handle_enter_room(client c, int room_num)
{
    if((c -> status) != NOT_CONNECTED)
    {
        if(room_num <  ROOM_COUNT && room_num > 0)
        {
            c -> room = rooms[i];
            c -> status =   IN_ROOM;

            client top = rooms[i] -> clients;
            mutex_lock (rooms[i]);  
            while
        }

        int message_length = sizeof(struct message_s);
        struct message_s *message = (struct message_s *)malloc(message_length);
        message->length = message_length;
        message->request_num = LIST_OF_ROOMS;
        return send_message(message, sockfd);
    }
}
