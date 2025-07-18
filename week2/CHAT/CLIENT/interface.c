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

void userInterface(int *sockfd)
{
    int sign_choice;
    int status = FAILURE;

    char room[BUF_SIZE];
    char room_to_join[BUF_SIZE];
    /*Sign Screen*/
    while(status == FAILURE)
    {
        sign_choice = choose_sign_option();
        status = sign_request(sign_choice,*sockfd);
    }
    status = FAILURE;

    /*rooms Screen*/
    while(choose_room_option(*sockfd,room)==FAILURE);

    /* chat screen*/
    
    pthread_t chat_thread;
    pthread_create(&chat_thread,NULL,(void*)recive_update,sockfd);
    pthread_detach(chat_thread);
    

    while(chat_screen(*sockfd) == SUCCESS)
    {

    }
    



    


}
int chat_screen(int sockfd)
{
    message send_msg,recv_msg;
    request rq = requests[NUM_SEND];
    return sendRequest(&rq,&send_msg,&recv_msg,sockfd);
   

}



int choose_room_option(int sockfd, char * room)
{
    message send_msg,recv_msg;
    request rq = requests[NUM_GET_ROOM];
    int status = sendRequest(&rq,&send_msg,&recv_msg,sockfd);
    if(status == FAILURE)
    {
        return FAILURE;
    }
    char rooms[ROOMS_AMOUNT][BUF_SIZE];
    char* token;
    char* rest = recv_msg.data;
    char input;
    int choice;
    int count = 0;
    while ((token = strtok_r(rest, " ", &rest)))
    {
        strncpy(rooms[count],token,BUF_SIZE);
        printf("%d. %s\n", count++ + 1,token);

    }

    
    rq = requests[NUM_JOIN_ROOM];
    status = sendRequest(&rq,&send_msg,&recv_msg,sockfd);
    if(status == SUCCESS)
    {
        printf("---JOINING ROOM---\n");

    }

    return status;
    
    
}


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

    char username[BUF_SIZE];
    char password[BUF_SIZE];
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
int getRooms_user(char *output, char * rooms, void * arg2)
{

 

    printf("\n---ROOMS---\n");
    printf("Select a room you would like to join. (select a number)\n");

    return SUCCESS;


}
int joinRoom_user       (char * room_name)
{
    printf("\n---JOIN ROOMS---\n");
    printf("enter the name of the room you would like to enter\n");
    fgets(room_name,BUF_SIZE,stdin);
    room_name[strcspn(room_name, "\n")] = 0;

    return SUCCESS;

}
int send_user           (char * msg,void * arg1, void* arg2)
{

    fgets(msg,MAXDATASIZE,stdin);
    msg[strcspn(msg, "\n")] = 0;
    if(strncmp(msg, " ",MAXDATASIZE) == 0)
    {
        return FAILURE;

    }
    return SUCCESS;


}
int exit_user           (void)
{
    printf("im out\n");

}
