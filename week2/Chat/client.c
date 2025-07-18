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

typedef enum{FIRST_HIRR,SECOND_HIRR,THIRD_HIRR} state;

typedef enum {SENT,NOT_SENT} msg_state;

typedef enum {SIGNED_IN,NOT_SIGNED_IN} sign_in_state;

static state chat_state_e = FIRST_HIRR;
static pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t message_mutex = PTHREAD_MUTEX_INITIALIZER;
static msg_state is_msg_sent_e = SENT;
static sign_in_state is_signed_in_e = NOT_SIGNED_IN;
static char g_message[200];
static pthread_cond_t message_cond = PTHREAD_COND_INITIALIZER;

#define IP "127.0.0.1"
#define PORT "8223"
#define FAIL -1
#define SUCCESS 0

/**
 * @brief this function establish socket connection
 * 
 * @return sockfd in case of success or fail if it failed
 */
int establish_connection()
{
    struct addrinfo hints, *res, *p;
    int sockfd, status_getaddr;
    int retval = SUCCESS;

    memset(&hints,0,sizeof hints);
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    if ((status_getaddr = getaddrinfo(IP,PORT,&hints,&res)) != 0)
    {
        fprintf(stderr, "get address info failed: %s", gai_strerror(status_getaddr));
        retval = FAIL;
    }
    if(retval == SUCCESS)
    {
        for(p = res; p != NULL; p = p->ai_next)
        {
            sockfd = socket(p->ai_family,p->ai_socktype,p->ai_protocol);
            if(sockfd == FAIL)
            {
                perror("socket");
                continue;
            }
            if(connect(sockfd, p->ai_addr, p->ai_addrlen) == FAIL)
            {
                perror("connect");
                close(sockfd);
                continue;
            }
            break;
        }
        freeaddrinfo(res);
        if(p == NULL)
        {
            fprintf(stderr,"failed to make a connection \n");
            retval = FAIL;
        }
        else
        {
            retval = sockfd;
        }
    }
    return retval;
}

/**
 * @brief this function gets the code func from the message it recieved
 * 
 * @param message_recived this is the message we recieved
 * @return the command number
 */
int command_number(char *message_recived)
{
    int command;
    char str_cmd[4];
    int index;
    for(index = 0; index < 3; index++)
    {
        *(str_cmd + index) = *(message_recived + index);
    }
    *(str_cmd + 3) = '\0';
    command = strtol(str_cmd, NULL, 10);
    return command;
}

/**
 * @brief this function prints the "opening" screen 
 * 
 */
void print_starting_pos()
{
    printf("to sign up type: 100 username password\n");
    printf("to sign in type: 101 username password\n");
}

/**
 * @brief this is the thread that responsible for recieving and making the string out put
 * 
 * @param arg int sockfd 
 * @return void* 
 */
void *server_connection_thread(void *arg)
{
    int sockfd = *(int*)arg;
    
    char message_buffer[100];
    int chars_recived;
    int command_num;
    int status;
    int messages_sent_from_server = 0;
    chars_recived = recv(sockfd, message_buffer, sizeof(message_buffer)-1, 0);
    if(chars_recived == -1)
    {
        perror("recv");
        exit(1);
    }
    while (chars_recived > 0)
    {
        message_buffer[chars_recived] = '\0';
        command_num = command_number(message_buffer);
        if(command_num == 200)
        {
            status = message_buffer[4] - '0';
            if(is_msg_sent_e == SENT)
            {
                if(status == SUCCESS)
                {
                    pthread_mutex_lock(&message_mutex);
                    char *message_ptr = "Signed up succefully:)";
                    strcpy(g_message, message_ptr);
                    is_msg_sent_e = NOT_SENT;
                    pthread_mutex_unlock(&message_mutex);
                    pthread_cond_signal(&message_cond);
                }
                else
                {
                    pthread_mutex_lock(&message_mutex);
                    char *message_ptr = "Sign up failed :(";
                    strcpy(g_message, message_ptr);
                    is_msg_sent_e = NOT_SENT;
                    pthread_mutex_unlock(&message_mutex);
                    pthread_cond_signal(&message_cond);
                }
            }
        }
        else if(command_num == 201)
        {
            status = message_buffer[4] - '0';
            if(is_msg_sent_e == SENT)
            {
                if(status == SUCCESS)
                {
                    pthread_mutex_lock(&message_mutex);
                    char *message_ptr = "Signed in succefully :)";
                    strcpy(g_message, message_ptr);
                    is_msg_sent_e = NOT_SENT;
                    is_signed_in_e = SIGNED_IN;
                    pthread_mutex_unlock(&message_mutex);
                    pthread_cond_signal(&message_cond);
                }
                else
                {
                    pthread_mutex_lock(&message_mutex);
                    char *message_ptr = "Sign in failed :(";
                    strcpy(g_message, message_ptr);
                    is_msg_sent_e = NOT_SENT;
                    pthread_mutex_unlock(&message_mutex);
                    pthread_cond_signal(&message_cond);
                }
            }
        }
        else if(command_num == 202)
        {
            if(is_msg_sent_e == SENT)
            {
                pthread_mutex_lock(&message_mutex);
                strcpy(g_message, message_buffer + 4);
                is_msg_sent_e = NOT_SENT;
                pthread_mutex_unlock(&message_mutex);
                pthread_cond_signal(&message_cond);
            }
        }
        else if(command_num == 203)
        {
            status = message_buffer[4] - '0';
            if(status == SUCCESS)
            {
                pthread_mutex_lock(&message_mutex);
                strcpy(g_message, "here is the chat's room, WELCOME\n");
                is_msg_sent_e = NOT_SENT;
                pthread_mutex_unlock(&message_mutex);
                pthread_cond_signal(&message_cond);
            }
        }
        else if(command_num == 204)
        {
            printf("%s\n", message_buffer + 4);
            fflush(stdout);
            pthread_mutex_lock(&message_mutex);
            is_msg_sent_e = NOT_SENT;
            pthread_mutex_unlock(&message_mutex);
            pthread_cond_signal(&message_cond);

            
        }
        else if(command_num == 211)
        {
            printf("%s\n", message_buffer + 4);
            fflush(stdout);
            pthread_mutex_lock(&message_mutex);
            is_msg_sent_e = NOT_SENT;
            pthread_mutex_lock(&state_mutex);
            chat_state_e = SECOND_HIRR;
            pthread_mutex_unlock(&message_mutex);
            pthread_cond_signal(&message_cond);
        }
        message_buffer[0] = '\0';
        chars_recived = recv(sockfd, message_buffer, sizeof(message_buffer)-1, 0);
    }
    return NULL;
}

/**
 * @brief this is the thread that responsible for the ui it sends the input to the server and manage the states
 * 
 * @param arg client sockfd
 * @return void* 
 */
void *ui_thread_func(void *arg)
{
    int sockfd = *(int*)arg;
    
    int num_events;
    int i = 0;
    print_starting_pos();

    while(1)
    {
        pthread_mutex_lock(&message_mutex);
        if (is_msg_sent_e == NOT_SENT)
        {
            printf("%s\n", g_message);
            is_msg_sent_e = SENT;
            g_message[0] = '\0';
            print_starting_pos();
        }
        pthread_mutex_unlock(&message_mutex);

        if(chat_state_e != SECOND_HIRR)
        {
            char response[30];
            i++;
            fgets(response, sizeof(response), stdin);  
            int end_location = strcspn(response, "\n");
            response[end_location] = '\0';
            int send_val;
            send_val = send(sockfd, response, strlen(response), 0);
            if(send_val<=0)
            {
                perror("send");
            }
            pthread_mutex_lock(&message_mutex);
            while(is_msg_sent_e == SENT) /* here we wait until the cond wait informs us there is a response to send*/
            {
                pthread_cond_wait(&message_cond, &message_mutex);
            }

            if(chat_state_e == FIRST_HIRR)/* if we just signing up/in */
            {
                if(is_signed_in_e == NOT_SIGNED_IN)/* means we are signing up*/
                {
                    if(is_msg_sent_e == NOT_SENT)
                    {
                        printf("%s\n", g_message);
                        print_starting_pos();

                        is_msg_sent_e = SENT;
                        g_message[0] = '\0';
                        pthread_mutex_unlock(&message_mutex);
                    }
                }
                else
                {
                    printf("%s\n", g_message);
                    g_message[0] = '\0';
                    is_msg_sent_e = SENT;
                    is_signed_in_e = SIGNED_IN;
                    pthread_mutex_unlock(&message_mutex);
                    pthread_mutex_lock(&state_mutex);
                    chat_state_e = SECOND_HIRR;
                    response[0] = '\0';
                    pthread_mutex_unlock(&state_mutex);
                }
            }

            if(chat_state_e == THIRD_HIRR)/* if we already got into a room chat*/
            {
                if(is_msg_sent_e == NOT_SENT )
                {
                    printf("%s\n", g_message);
                    g_message[0] = '\0';
                    is_msg_sent_e = SENT;
                    pthread_mutex_unlock(&message_mutex);
                }
            }
        }

        if(chat_state_e == SECOND_HIRR) /* the menu to select a room*/
        {
            send(sockfd, "102", 3, 0);
            pthread_mutex_lock(&message_mutex);
            while(is_msg_sent_e == SENT)
            {
                pthread_cond_wait(&message_cond, &message_mutex);
            }
            printf("to join a room type 103 username room number\n");
            printf("which room you would like to join?\n");
            printf("%s\n", g_message);
            g_message[0] = '\0';
            is_msg_sent_e = SENT;
            pthread_mutex_unlock(&message_mutex);
            pthread_mutex_lock(&state_mutex);
            chat_state_e = THIRD_HIRR;
            pthread_mutex_unlock(&state_mutex);
        }
    }
    return NULL;
}

int main (void)
{
    int retval = SUCCESS;
    int sockfd;
    sockfd = establish_connection();
    int *arg = malloc(sizeof *arg);
    *arg = sockfd;
    if(sockfd == FAIL)
    {
        fprintf(stderr, "couldnt connect to server\n");
        retval = FAIL;
    }
    pthread_t session;
    pthread_create(&session, NULL, server_connection_thread, arg);
    pthread_t ui_thread;
    pthread_create(&ui_thread, NULL, ui_thread_func, arg);
    pthread_join(ui_thread, NULL);
    return retval;
}
