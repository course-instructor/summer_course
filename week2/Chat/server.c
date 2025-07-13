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

enum connection  {DISCONNECTED, CONNECTED};  

#define FAIL -1
#define SUCCESS 0

#define MAX_USERS 50
#define PORT "4096"

struct room {
    int  room_id;
    int  members[MAX_USERS];
    int  member_count;
    pthread_mutex_t mutex;
};

struct client {
    int room_id;
    int client_fd;
};

static struct room rooms[5];
static int number_clients = 0;
static pthread_mutex_t clients_mutex;

void init_rooms()
{
    int room_id;
    for(room_id = 0; room_id < 5; room_id++)
    {
        rooms[room_id].room_id = room_id + 1;
        rooms[room_id].member_count = 0;
        memset(rooms[room_id].members,0,sizeof rooms[room_id].members);
        pthread_mutex_init(&rooms[room_id].mutex, NULL);
    }
}

int get_listener_socket(void)
{
    printf("start of get listener func\n");
    int listener = 0;
    int yes=1;
    int retval;

    struct addrinfo hints, *ai, *p;

    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if ((retval = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(retval));
        exit(1);
    }
    printf("after getaddrinfo%d\n",listener);

    for(p = ai; p != NULL; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        printf("listener value in the beginning of for: %d\n",listener);
        if (listener < 0) {
            printf("inside the if\n");

            continue;
        }

      
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
        int resault;

        if (resault = (listener, p->ai_addr, p->ai_addrlen) < 0) {
            printf("bind resault : %s", resault);
            close(listener);
            continue;
        }
        
        break;

    }
    printf("after for loop %d\n",listener);

    freeaddrinfo(ai); 

    // if (p == NULL) {
    //     printf("p == null check");
    //     return -1;
    // }

    
    // if (listen(listener, 10) == -1)
    // {
    //     printf("listenr check");
    //     return -1;
    // }

    printf(" returned listener is: %d\n",listener);
    return listener;
}

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

int sign_in_request(char *command)
{
    int retval = SUCCESS;
    FILE *users_pointer;
    users_pointer = fopen("users.txt", "r");
    if (users_pointer == NULL)
    {
        retval = FAIL;
    }
    if (retval == SUCCESS)
    {
        char *username;
        command += 4;
        char *input_username = command;
        char *input_password;
        char *space = strchr(input_username, ' ');
        if (!space) { 
            fclose(users_pointer);
            return FAIL;
        }
        *space = '\0';
        input_password = space + 1;

        char line[50];
        while (fgets(line, sizeof(line), users_pointer))
        {
            username = strtok(line, ',');
            if (strcmp(username, input_username) == 0)
            {
                char *password = strtok(NULL, ',');
                char *end = strchr(password, '\n');
                if (end)/* swithcing the \n to \0 */
                {
                    *end = '\0';
                }
                if (strcmp(password, input_password) == 0)
                    retval = SUCCESS;
                else
                    retval = FAIL;
                
            }
        }
    }
    fclose(users_pointer);
    return retval;
}

char *send_room_list(void)
{
    static char rooms_list[256];
    int pos = 0;
    int room_index;

    for (int room_index = 0; room_index < 5; room_index++) 
    {
        pos += sprintf(rooms_list + pos, "%d,%d\n", rooms[room_index].room_id, rooms[room_index].member_count);
    }
    return rooms_list;
}

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
        pthread_mutex_lock(&rooms[room_id_num-1].mutex);
        rooms[room_id_num-1].member_count++;
        rooms[room_id_num-1].members[rooms[room_id_num-1].member_count - 1] = client_fd;
        client->room_id = room_id_num;
        pthread_mutex_unlock(&rooms[room_id_num - 1].mutex);
    }
    return retval;
    

}

char * send_to_group(char * command, struct client *client)
{
    while(isdigit(*command))
    {
        command++;
    }
    int room_id_num = client->room_id;
    static char message[200];
    sprintf(message,"%s\n",command);
    return message;
}

char * remove_from_group(char *command, struct client *client)
{
    command+=4;
    int room_num_id = client->room_id;
    pthread_mutex_lock(&rooms[room_num_id - 1].mutex);
    rooms[room_num_id - 1].member_count--;
    int index_in_members;
    for(index_in_members = 0; index_in_members < rooms[room_num_id - 1].member_count;index_in_members++)
    {
        if(rooms[room_num_id - 1].members[index_in_members] == client->client_fd)
        {
            break;
        }
    }
    if(index_in_members < rooms[room_num_id - 1].member_count)
    {
        int second_idx;
        for(second_idx = index_in_members; second_idx<rooms[room_num_id - 1].member_count -1 ; second_idx++)
        {
            rooms[room_num_id - 1].members[second_idx] = rooms[room_num_id - 1].members[second_idx + 1];

        }
    }
    pthread_mutex_unlock(&rooms[room_num_id - 1].mutex);
    static char response[20];
    sprintf(response,"%s left the room",command);
    return response;
}


void *chat_session(void *arg)
{
    printf("session begin");
    struct client *client = arg;
    char message_buffer[100];
    int chars_recived;
    int size_to_send;
    int command_num;
    int status_func;
    int is_connected = DISCONNECTED;
    chars_recived = recv(client->client_fd,message_buffer,sizeof(message_buffer)-1, 0);
    printf("%d" ,chars_recived);
    while(chars_recived > 0)
    {
        int status;
        char *response = malloc(200);
        message_buffer[chars_recived] = '\0';
        printf("%s",message_buffer);
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
                    strcpy(response,returned_list);
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
                *response = "204 ";
                strcat(response,send_to_group(message_buffer,client));
                size_to_send = strlen(response);
                int index_member_in_group;
                for(index_member_in_group = 0; index_member_in_group < rooms[client->room_id - 1].member_count;index_member_in_group++)
                {
                    send(rooms[client->room_id - 1].members[index_member_in_group],response,size_to_send,0);
                }
            }
            else if(command_num == 105)
            {  
                *response ="205 ";
                strcat(response,remove_from_group(message_buffer,client));
                size_to_send = strlen(response);
                int index_member_in_group;
                for(index_member_in_group = 0; index_member_in_group < rooms[client->room_id - 1].member_count;index_member_in_group++)
                {
                    send(rooms[client->room_id - 1].members[index_member_in_group],response,size_to_send,0);
                }
                
            }

        }
        free(response);
        chars_recived = recv(client->client_fd, message_buffer, sizeof(message_buffer) - 1, 0);
        
    }
    close(client->client_fd);
    pthread_mutex_lock(&clients_mutex);
    number_clients--;
    pthread_mutex_unlock(&clients_mutex);
    free(client);
    return NULL;
}

int main()
{

    init_rooms();
    pthread_mutex_init(&clients_mutex,NULL);
    int retval = SUCCESS;
    int listener;
    

    listener = get_listener_socket();
    printf("%d",listener);

    struct sockaddr_storage client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int *newfd = malloc(sizeof(int));

    *newfd = accept(listener, (struct sockaddr *)&client_addr, &client_addr_len);
    
    if (*newfd == -1) {
        perror("accept failed");
        free(newfd);
        return;
    }
        pthread_mutex_lock(&clients_mutex);
        if(number_clients<MAX_USERS)
        {
            number_clients++;
            pthread_mutex_unlock(&clients_mutex);
            struct client *client = malloc(sizeof *client);
            client->client_fd = newfd;
            client->room_id   = 0;
            pthread_t session;
            printf("line above thread");
            pthread_create(&session, NULL, chat_session,client);
            pthread_detach(&session);
        }
        else
        {
            char *message_error = "currently server full\n";
            send(newfd,message_error,strlen(message_error),0);
            close(newfd);
            
        }
        
    
    
}
