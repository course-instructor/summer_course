#define _XOPEN_SOURCE 600 
/*the reason why I left it here anyway: https://stackoverflow.com/a/23961526 , or basiclly to get rid of annoying warning.
I hate those silly warrnings D:< */
#include <stdio.h>
#include "client.h"
#include <ctype.h>
static const unsigned int login_args	[] 	= {USERNAME_SIZE, PASSWORD_SIZE};
static const unsigned int register_args	[] 	= {USERNAME_SIZE, PASSWORD_SIZE};
static const unsigned int get_room_args[] 	= {ROOM_SIZE*ROOMS_AMOUNT};
static const unsigned int join_room_args[] 	= {ROOM_SIZE};
static const unsigned int send_args		[] 	= {MSG_SIZE};

static const request requests[AMOUNT_OF_REQUESTS] =
{
/* request code     |   reply code    |   access level | num argumants| size of argumants |server's operation    |		*/
	{REQUEST_REGISTER	,REPLY_REGISTER	,	EVERY_USER		,	2	,register_args	,	(void *)&register_user	},
	{REQUEST_LOGIN   	,REPLY_LOGIN	,	EVERY_USER		,	2	,login_args		,	(void *)&login_user		},
	{REQURST_GET_ROOM	,REPLY_GET_ROOM	,	CONNECTED_USER	,	1	,get_room_args	,	(void *)&getRooms_user	},
	{REQUEST_JOIN_ROOM	,REPLY_JOIN_ROOM,	CONNECTED_USER	, 	1	,join_room_args ,	(void *)&joinRoom_user	},
	{REQUEST_SEND		,REPLY_SEND		,	IN_ROOM_USER	,	1	,send_args		,	(void *)&send_user		},
	{REQUEST_EXIT		,REPLY_EXIT		,	IN_ROOM_USER	,	0	,NULL   		,	(void *)&exit_user		}
};
int is_chat_open=0;

/**
 * @brief main thread to handle users input and requests
 */
void userInterface(int *sockfd)
{
    int sign_choice;
    int status = FAILURE;
    pthread_t chat_thread;
    /*Sign Screen*/
    while(status == FAILURE)
    {
        sign_choice = choose_sign_option();
        status = sign_request(sign_choice,*sockfd);
    }
    status = FAILURE;

    /*rooms Screen*/
    while(1)
    {
        while(choose_room_option(*sockfd)==FAILURE);

        /* chat screen*/
        
        is_chat_open = 1;
        pthread_create(&chat_thread,NULL,(void*)recive_update,sockfd);
        pthread_detach(chat_thread);
        
        printf("(To exit chat write [%s].)\n",EXIT_COMMAND);
        while(is_chat_open == 1 && chat_screen(*sockfd) == SUCCESS);
        pthread_cancel(chat_thread);

    }


    
   

}


/**
 * @brief send messeges
 */
int chat_screen(int sockfd)
{
    int status;
    message send_msg,recv_msg;
    request rq;
    rq = requests[NUM_SEND];

    sendRequest(&rq,&send_msg,&recv_msg,sockfd);
    status = recv_msg.status;
    if(recv_msg.opcode == REPLY_EXIT)
    {
        printf("THIS WAY\n");
        is_chat_open = 0;
        status = DISCONNECT;
    }
    return status;
}
/**
 * @brief choose room to join
 */
int choose_room_option(int sockfd)
{
    message send_msg,recv_msg;
    request rq = requests[NUM_GET_ROOM];

    char rooms[ROOMS_AMOUNT][BUF_SIZE];
    char* token;
    char* rest = recv_msg.data;
    int count = 0;
    sendRequest(&rq,&send_msg,&recv_msg,sockfd);
    int status = recv_msg.status;
    if(status == SUCCESS)
    {
        while ((token = strtok_r(rest, " ", &rest)))
        {
            strncpy(rooms[count],token,BUF_SIZE);
            printf("%d. %s\n", count++ + 1,token);

        }

        rq = requests[NUM_JOIN_ROOM];
        sendRequest(&rq,&send_msg,&recv_msg,sockfd);
        status = recv_msg.status;
        if(status == SUCCESS)
        {
            printf("---JOINING ROOM---\n");

        }

    }
    else
    {
        fprintf(stderr,"ERORR: %s",recv_msg.data);
    }


    return status;   
}

/**
 * @brief choose between login and sign up
 */
int choose_sign_option(void)
{
    char input;
    char answers[] = {'1','2','3'};
    printf("Would you like to login or sign up (enter a number)?\n");
    printf("1. login\n");
    printf("2. sign up\n");
    printf("3. exit\n");
    input = fgetc(stdin);
    while(input !=answers[0] && input !=answers[1] && input != answers[2])
    {
        printf("incorrect input! please try again.\n");
        input = fgetc(stdin);
    }
    
    return input - '0';
}
/**
 * @brief sigining in by user's choice
 */
int sign_request(int choice,int sockfd)
{
    message send_msg;
    message recv_msg;

    request rq;

    enum screens_e
    {
        LOGIN_SCREEN = 1,
        SIGN_UP_SCREEN = 2,
        EXIT_SCREEN  = 3

    };


    switch (choice)
    {
    case LOGIN_SCREEN:
        rq = requests[NUM_LOGIN];
        break;

    case SIGN_UP_SCREEN:
        rq = requests[NUM_REIGISTER];
        break;

    case EXIT_SCREEN:
        printf("Goodbye!\n");
        exit(1);
        break;

    default:
        break;
    }
    sendRequest(&rq,&send_msg,&recv_msg,sockfd);

    if(recv_msg.opcode == rq.replay_code)
    {
       if(recv_msg.status == SUCCESS) 
       {
            printf("logged in succesfuly!!\n");
            return SUCCESS;
       }
       else
       {
            printf("error : %s\n",recv_msg.data);
            return FAILURE;
       }
    }
    return SUCCESS;

}


int contains_whitespace(const char *str) {
    while (*str) {
        if (isspace((unsigned char)*str)) {
            return 1;
        }
        str++;
    }
    return 0; 
}
/**
 * @brief get new user from input
 */
int register_user       (char * output, char * username , char * password)
{
    int isvalid = 0;
    fgets(username,sizeof(username),stdin);

    printf("---SIGN UP---\n");
    while(isvalid == 0)
	{
		printf("USERNAME:\n");
        fgets(username,sizeof(username),stdin);
        username[strcspn(username, "\n")] = 0;

		if(username == NULL)
		{
			printf("Invalid input\n");
		}
		else if(strlen(username) <=3)
		{
			printf("Username is too short!\n");
		}
        else if(contains_whitespace(username)== 1)
        {   
            printf("Username cant contain spaces\n");
        }
		else
		{
			isvalid =1;
		}
	}

    isvalid = 0;

    while(isvalid == 0)
	{
		printf("PASSWORD:\n");
        fgets(password,sizeof(password),stdin);
        password[strcspn(password, "\n")] = 0;

		if(password == NULL)
		{
			printf("Invalid input\n");
		}
		else if(strlen(password) <=3)
		{
			printf("Password is too short!\n");
		}
        else if(contains_whitespace(password)== 1)
        {   
            printf("Password cant contain spaces\n");
        }
		else
		{
			isvalid =1;
		}
	}
    snprintf(output,MAXDATASIZE,"%s\t%s",username,password);
    return SUCCESS;

}
/**
 * @brief get existing user from input
 */
int login_user          (char * output, char * username , char * password)
{
    printf("---LOGIN---\n");
    int isvalid = 0;
    fgets(username,sizeof(username),stdin);
    while(isvalid == 0)
	{
		printf("USERNAME:\n");
        fgets(username,sizeof(username),stdin);
        username[strcspn(username, "\n")] = 0;

		if(username == NULL)
		{
			printf("Invalid input\n");
		}
		else if(strlen(username) <=3 && strlen(username) !=0)
		{
			printf("Username is too short!\n");
		}
        else if(contains_whitespace(username)== 1)
        {   
            printf("Username cant contain spaces\n");
        }
		else
		{
			isvalid =1;
		}
	}

    isvalid = 0;
    fflush(NULL);
    while(isvalid == 0)
	{
		printf("PASSWORD:\n");
        fgets(password,sizeof(password),stdin);
        password[strcspn(password, "\n")] = 0;

		if(password == NULL)
		{
			printf("Invalid input\n");
		}
		else if(strlen(password) <=3)
		{
			printf("Password is too short!\n");
		}
        else if(contains_whitespace(password)== 1)
        {   
            printf("Password cant contain spaces\n");
        }
		else
		{
			isvalid =1;
		}
	}
    snprintf(output,MAXDATASIZE,"%s\t%s",username,password);
    return SUCCESS;


}
/**
 * @brief rooms title
 */
int getRooms_user(void)
{

    printf("\n---ROOMS---\n");

    return SUCCESS;


}
/**
 * @brief get room to join from input
 */
int joinRoom_user       (char * room_name)
{
    printf("\n---JOIN ROOMS---\n");
    printf("enter the name of the room you would like to enter\n");
    fgets(room_name,BUF_SIZE,stdin);
    room_name[strcspn(room_name, "\n")] = 0;

    return SUCCESS;

}
/**
 * @brief get messege to sent from input
 */
int send_user           (char * output)
{
    fgets(output,MAXDATASIZE,stdin);
    output[strcspn(output, "\n")] = 0;
    if(strncmp(output, "",MAXDATASIZE) == 0)
    {
        return FAILURE;

    }
    return SUCCESS;


}
int exit_user           (void)
{
    printf("im out\n");
    return SUCCESS;

}
/**
 * @brief thread for reading incoming messeges
 */
void recive_update(int *sockfd)
{
    message recv_msg;
    
    while(is_chat_open == 1)
    {
        if(receive_message(*sockfd,&recv_msg) < 0)
		{
			break;
		}
        if(recv_msg.opcode == REPLY_UPDATE)
        {
            printf("%s\n",recv_msg.data);
        }
        else if(recv_msg.opcode == REPLY_EXIT)
        {
            is_chat_open = 0;
			printf("[Exiting room...]");
        }
    }

}