#ifndef CLIENT_H
#define CLIENT_H
#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#define EXIT_COMMAND "/exit"

#define MAXDATASIZE 256 // max number of bytes we can get at once 
#define ROOMS_AMOUNT 5
#define BUF_SIZE 16
#define AMOUNT_OF_REQUESTS 6

#define USERNAME_SIZE BUF_SIZE*sizeof(char)
#define PASSWORD_SIZE BUF_SIZE*sizeof(char)
#define ROOM_SIZE     BUF_SIZE*sizeof(char)
#define MSG_SIZE      MAXDATASIZE*sizeof(char)
enum request_code_e
{
	REQUEST_REGISTER	= 100,
	REQUEST_LOGIN  		= 101,
	REQURST_GET_ROOM	= 102,
	REQUEST_JOIN_ROOM	= 103,
	REQUEST_SEND		= 104,
	REQUEST_EXIT		= 105	
};
enum reply_code_e
{
	REPLY_REGISTER  	= 200,
	REPLY_LOGIN 		= 201,
	REPLY_GET_ROOM	    = 202,
	REPLY_JOIN_ROOM	    = 203,
	REPLY_SEND	    	= 204,
	REPLY_UPDATE		= 205,
	REPLY_EXIT  		= 211	
};
enum request_num_e
{
    NUM_REIGISTER       =   0,
	NUM_LOGIN           =   1,
    NUM_GET_ROOM        =   2,
    NUM_JOIN_ROOM       =   3,
    NUM_SEND            =   4,
    NUM_EXIT            =   5
};
enum access_hierarchy_e
{
	EVERY_USER		=   1,
	CONNECTED_USER	=	2,
	IN_ROOM_USER    =   3
};
enum status_e
{
    SUCCESS =  0,
    FAILURE  = -1,
	DISCONNECT = -2
};


typedef struct messege_t
{
    u_int32_t opcode; /* messege code */
	enum status_e status; /*operation status (SUCCESS/FAILURE/etc)*/
	char data[MAXDATASIZE]; /*the data itself , not required*/
}message;


/*Represent a converstion between client and server in which client sends a request, server processing it, and sends replay*/
typedef struct request_t
{
    enum request_code_e request_code; 
    enum reply_code_e replay_code;
    enum access_hierarchy_e access;
    int argc;
    const unsigned int *args_size;
    int (*operation)(void* , void*,void *);
}request;


int establish_connection(char * address,int *sockfd);
int clientSide(int argc, char *argv[]);
int sendRequest(request * rq, message * send_msg,message * recv_msg,int sockfd);
int receive_message(int sockfd, message *msg);
int sendRequest(request * rq, message * send_msg,message * recv_msg,int sockfd);
int sign_request(int choice,int sockfd);


void userInterface(int * sockfd);
int choose_sign_option(void);
int choose_room_option(int sockfd);
int chat_screen(int sockfd);


/*Chat */

void recive_update(int * sockfd);

/*local logical functions*/

int register_user       (char * output, char * username , char * password);
int login_user			(char * output,char * username , char * password);
int getRooms_user		(void);
int joinRoom_user       (char * room_name);
int send_user           (char * output);
int exit_user           (void);

#endif