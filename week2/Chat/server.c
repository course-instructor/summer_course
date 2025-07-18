#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <ctype.h>
enum connection  {DISCONNECTED, CONNECTED};  

#define FAIL -1
#define SUCCESS 0

#define MAX_USERS 50
#define PORT "8223"

struct client 
{
    int room_id;
    int client_fd;
    char username[10];
};

struct room 
{
    int  room_id;
    struct client  client_list[MAX_USERS];
    int  member_count;
    pthread_mutex_t mutex;
};

static struct room rooms_t[5];
static int g_number_clients;
static pthread_mutex_t clients_mutex;


/**
 * @brief this function initialize all the rooms
 * 
 */
void init_rooms()
{
    int room_id;
    for(room_id = 0; room_id < 5; room_id++)
    {
        rooms_t[room_id].room_id = room_id + 1;
        rooms_t[room_id].member_count = 0;
        memset(rooms_t[room_id].client_list,0,sizeof rooms_t[room_id].client_list);
        pthread_mutex_init(&rooms_t[room_id].mutex, NULL);
    }
}

/**
 * @brief Get the listener socket object
 * 
 * @return int sockfd
 */
int get_listener_socket(void)
{
    int listener = 0;
    const int yes = 1;
    int retval;

    struct addrinfo hints, *ai, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;
    if ((retval = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) 
    {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(retval));
        exit(1);
    }

    for (p = ai; p != NULL; p = p->ai_next)
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) 
        {
            continue;
        }

        if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) 
        {
            perror("setsockopt");
            close(listener);
            continue;
        }

        int res = bind(listener, p->ai_addr, p->ai_addrlen);
        if (res < 0) {
            perror("bind");
            close(listener);
            continue;
        }

        break;
    }


    freeaddrinfo(ai);

    if (p == NULL) {
        printf("p == null check");
        return -1;
    }

    if (listen(listener, 10) < 0)
    {
        perror("listen");
        close(listener);
        return -1;
    } else 
    {

    }

    return listener;
}

/**
 * @brief this function puts the command number into a value as a int
 * 
 * @param message_recived this is the message we recieved from the client
 * @return int command
 */
int command_number(char *message_recived)
{
    int command;
    char str_cmd[4];
    int index;
    for(index = 0; index < 3;index++)
    {
        *(str_cmd + index) = *(message_recived + index);
    }
    *(str_cmd + 3) = '\0';
    command = strtol(str_cmd,NULL,10);
    return command;
}

/**
 * @brief this function gets the username and password and writes them into the users file
 * 
 * @param command this is the message we recieved
 * @return int success/fail
 */
int sign_up_request(char *command)
{
    int retval = SUCCESS;
    FILE *users_pointer;
    users_pointer =fopen("users.txt" , "a"); 
    if(users_pointer == NULL)
    {
        retval = FAIL;
    }
    if(retval == SUCCESS)
    {

        command+=4;
        char *username = command;
        char *password;
        while(!isspace(*command))
        {
            command++;
        }
        *command = '\0';
        password = command+1;
        if(*username && *password)
        {
            fprintf(users_pointer,"%s,%s\n",username,password);
        }
        else
        {
            retval = FAIL;
        }
        fclose(users_pointer);
    }
    
    return retval;
}

/**
 * @brief this function tries to sign in a client
 * 
 * @param command the message we recieved
 * @return int that represents sign in successfully/unsuccesfully
 */
int sign_in_request(char *command)
{
    char *input_username;
    char *input_password;
    int retval = FAIL;            
    FILE *users_pointer = fopen("users.txt", "r");
    if (!users_pointer)  
    {
        printf("couldnt open file\n");
        return retval;
    }         
    command += 4;
    input_username = command;
    while (!isspace(*command))
    {
        command++;
    }
    *command = '\0';
    input_password = command + 1;
    char line[128];
    while (fgets(line, sizeof(line), users_pointer))
    {
        line[strcspn(line,"\n")] = '\0';
        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");

        int username_check = strcmp(input_username,username);
        int password_check = strcmp(input_password,password);
        if (username_check == 0 && password_check == 0)
        {
            retval = SUCCESS; 
            break;
        }
    }

    fclose(users_pointer);
    return retval;
}

/**
 * @brief we create a list of the room number and the number of clients inside the room
 * 
 * @return char* we return the list of rooms
 */
char *send_room_list(void)
{
    static char rooms_list[256];
    int pos = 0;
    int room_index;

    for (int room_index = 0; room_index < 5; room_index++) 
    {
        pos += sprintf(rooms_list + pos, "%d,%d\n", rooms_t[room_index].room_id, rooms_t[room_index].member_count);
    }
    return rooms_list;
}

/**
 * @brief here we add the client to the room he asked for 
 * 
 * @param command the message we recieved
 * @param client_fd the file director of the client
 * @param client struct of the client to assign him the room
 * @return int success or fail
 */
int add_to_room(char *command,int client_fd, struct client *client)
{
    int retval = SUCCESS;
    command+=4;
    char *room_id;
    room_id = command;
    int room_id_num;
    while(!isspace(*room_id))
    {
        room_id++;
    }
    
    room_id_num = strtol(room_id,NULL,10);
    if(room_id_num > 5 || room_id_num < 1)
    {
        retval = FAIL;
    }
    else
    {
        pthread_mutex_lock(&rooms_t[room_id_num-1].mutex);
        rooms_t[room_id_num-1].member_count++;
        rooms_t[room_id_num-1].client_list[rooms_t[room_id_num-1].member_count - 1].client_fd = client_fd;
        client->room_id = room_id_num;
        pthread_mutex_unlock(&rooms_t[room_id_num - 1].mutex);
    }
    return retval;
    

}
static char message[200];

/**
 * @brief here we put in message buffer the message we want to send to all the clients in the room
 * 
 * @param command the message we recieved
 * @param client the struct of the client itself
 * @return char* we return the message buffer
 */
char * send_to_group(char * command, struct client *client)
{
    while(isdigit(*command))
    {
        command++;
    }
    int room_id_num = client->room_id;

    sprintf(message,"%s",command);
    return message;
}

/**
 * @brief we remove the client from its room
 * 
 * @param client we recieve the struct of the client and we change his fields
 */
void remove_from_group( struct client *client)
{
    
    int room_num_id = client->room_id;
    pthread_mutex_lock(&rooms_t[room_num_id - 1].mutex);
    rooms_t[room_num_id - 1].member_count--;
    int index_in_members;
    for(index_in_members = 0; index_in_members < rooms_t[room_num_id - 1].member_count;index_in_members++)
    {
        if(rooms_t[room_num_id - 1].client_list[index_in_members].client_fd == client->client_fd)
        {
            break;
        }
    }
    if(index_in_members < rooms_t[room_num_id - 1].member_count)
    {
        int second_idx;
        for(second_idx = index_in_members; second_idx<rooms_t[room_num_id - 1].member_count -1 ; second_idx++)
        {
            rooms_t[room_num_id - 1].client_list[second_idx] = rooms_t[room_num_id - 1].client_list[second_idx + 1];

        }
    }
    pthread_mutex_unlock(&rooms_t[room_num_id - 1].mutex);
}

/**
 * @brief this is the function that the thread run
 * 
 * @param arg is the argument from the main in this case the client struct
 * @return void* 
 */
void *chat_session(void *arg)
{
    struct client *client = arg;
    char message_buffer[100];
    int chars_recived;
    int size_to_send;
    int command_num;
    int status_func;
    int is_connected = DISCONNECTED;
    chars_recived = recv(client->client_fd,message_buffer,sizeof(message_buffer)-1, 0);
    while(chars_recived > 0)
    {
        int status;
        char *response = malloc(200);
        message_buffer[chars_recived] = '\0';

        command_num = command_number(message_buffer);

        if(!is_connected)
        {
            if(command_num == 100)
            {
                
                status = sign_up_request(message_buffer);
                
                size_to_send =  sprintf(response, "200 %d\n", status);
                send(client->client_fd,response,size_to_send,0);
                
                
                    
            }
            else if(command_num == 101)
            {
                status = sign_in_request(message_buffer);
                
                
                size_to_send = sprintf(response, "201 %d\n", status);
                send(client->client_fd,response,size_to_send,0);
                is_connected = CONNECTED;
            }

        }
        else
        {
            if(command_num == 102)
            {
                char *returned_list;
                returned_list = send_room_list();
                if(returned_list == NULL)
                {
                    size_to_send = sprintf(response,"-1");
                    send(client->client_fd,response,size_to_send,0);
                }
                else
                {
                    strcat(response,"202 ");
                    strcat(response,returned_list);
                    size_to_send = strlen(response);
                    send(client->client_fd,response,size_to_send,0);
                }
            }
            else if(command_num == 103)
            {
               status = add_to_room(message_buffer,client->client_fd,client);
               size_to_send = sprintf(response, "203 %d\n", status);
               send(client->client_fd,response,size_to_send,0);
               
            }
            else if(command_num == 104)
            {
                strcpy(response,"204 ");
                strcat(response,send_to_group(message_buffer,client));
                size_to_send = strlen(response);
                int index_member_in_group;
                for(index_member_in_group = 0; index_member_in_group < rooms_t[client->room_id - 1].member_count;index_member_in_group++)
                {
                    send(rooms_t[client->room_id - 1].client_list[index_member_in_group].client_fd,response,size_to_send,0);
                }
            }
            else if(command_num == 105)
            {
                strcpy(response ,"211 ");
                strcat(response,send_to_group(message_buffer,client));
                strcat(response," left the room\n");
                size_to_send = strlen(response);
                int index_member_in_group;
                for(index_member_in_group = 0; index_member_in_group < rooms_t[client->room_id - 1].member_count;index_member_in_group++)
                {
                    send(rooms_t[client->room_id - 1].client_list[index_member_in_group].client_fd,response,size_to_send,0);
                }
                remove_from_group(client);
                
            }

        }
        free(response);
        memset(response,0,strlen(response));
        memset(message_buffer,0,strlen(message_buffer));
        chars_recived = recv(client->client_fd, message_buffer, sizeof(message_buffer) - 1, 0);
        
    }
    close(client->client_fd);
    pthread_mutex_lock(&clients_mutex);
    g_number_clients--;
    pthread_mutex_unlock(&clients_mutex);
    free(client);
    return NULL;
}

int main()
{

    g_number_clients = 0;
    init_rooms();
    pthread_mutex_init(&clients_mutex,NULL);
    int retval = SUCCESS;
    int listener;
    

    listener = get_listener_socket();


    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int newfd;

    while(1)
    {

        newfd = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);

        if (newfd == -1)
        {
            perror("accept failed");
            return 0;
        }

        pthread_mutex_lock(&clients_mutex);
        if(g_number_clients<MAX_USERS)
        {
            g_number_clients++;
            pthread_mutex_unlock(&clients_mutex);
            struct client *client = malloc(sizeof *client);
            client->client_fd = newfd;
            client->room_id   = 0;
            pthread_t session;

            pthread_create(&session, NULL, chat_session,client);
            pthread_detach(session);
        }
        else
        {
            char *message_error = "currently server full\n";
            send(newfd,message_error,strlen(message_error),0);
            close(newfd);
            
        }
    }
}
