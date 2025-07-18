#ifndef SERVER_H
#define SERVER_H
#define BUF_SIZE 16
#define USERNAME_BUFFER_SIZE 16
#define PASSWORD_BUFFER_SIZE 16
#define ROOMS_AMOUNT 5
#define FILE_NAME_BUFF_SIZE 16
#define MAXDATASIZE 256 // max number of bytes we can get at once 
#define MAX_CONNECTIONS 50

enum status_e
{
    SUCCESS =  0,
    FAILURE  = -1
};
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
    REPLY_UPDATE        = 205,
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

typedef struct request_t
{
    enum request_code_e request_code;
    enum reply_code_e replay_code;
    enum access_hierarchy_e access;
    int argc;
    const unsigned int *args_size;
    int (*operation)(int, void* , void*, void *);
}request;

typedef struct user_t
{
    int sockfd;
	char username[BUF_SIZE];
	char password[PASSWORD_BUFFER_SIZE];
	char room[BUF_SIZE];
    int room_id;
    enum access_hierarchy_e access_level;

}user;
typedef struct room_t
{
    char room_name[16];
    int  members_count;
    char room_file_path[24];
}room;

typedef struct messege_t
{
    u_int32_t opcode;
	enum status_e status;
	char data[MAXDATASIZE];
}message;

struct args_s {
    int sockfd;
    int thread_id;
};


/* Request Functions */

int register_user		(int thread_id ,char *output ,char * username , char * password);
int login_user			(int thread_id ,char * output,char * username , char * password);
int getRooms_user		(int thread_id ,char *output, void * arg1, void * arg2);
int joinRoom_user       (int thread_id ,char * username, char * room_name);
int send_user			(int thread_id, char * output,char * msg);
int exit_user           (int thread_id);

int userHandle(void * arguments);

int receive_message(int sockfd, message *msg, size_t data_size);
int send_message(int sockfd, const message *msg, size_t data_size);
int multicast_messege(int thread_id,char * msg_data,int msg_length);

int getRooms_request(int sockfd);
int joinRoom_request(int sockfd,user cur_user);


int writeUser           (user * new_user);
int findUserByUsername  (char * username, user *saved_user);
void getRooms           (room * rooms[ROOMS_AMOUNT]);
int getRoomByName       (room * room_output,char * room_name);


#endif

