#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h> 
#include "server.h"
#define USERS_FILE_PATH "users.txt"
#define ROOM_FOLDER_PATH "./ROOMS"

#define ERROR_MESSEGE_NAME_UNSPECIFIED  "name is not specified."
#define ERROR_MESSEGE_NAME_EXISTS "user with this name does not exists."
#define ERROR_MESSEGE_INCORRECT_PASSWORD "incorrect password."

#define AMOUNT_OF_REQUESTS 7


#define USERNAME_SIZE USERNAME_BUFFER_SIZE*sizeof(char)
#define PASSWORD_SIZE PASSWORD_BUFFER_SIZE*sizeof(char)
#define ROOM_SIZE FILE_NAME_BUFF_SIZE*sizeof(char)
#define MSG_SIZE BUF_SIZE*sizeof(char)
static const unsigned int login_args	[] 	= {USERNAME_SIZE, PASSWORD_SIZE};
static const unsigned int register_args	[] 	= {USERNAME_SIZE, PASSWORD_SIZE};
static const unsigned int join_room_args[] 	= {ROOM_SIZE};
static const unsigned int send_args		[] 	= {USERNAME_SIZE, MSG_SIZE};
static const unsigned int exit_args		[] 	= {USERNAME_SIZE};

static const request requests[AMOUNT_OF_REQUESTS] =
{
/* request code     |   reply code    |   access level | num argumants| size of argumants |server's operation    |		*/
	{REQUEST_REGISTER	,REPLY_REGISTER	,	EVERY_USER		,	2	,register_args	,	(void *)&register_user	},
	{REQUEST_LOGIN   	,REPLY_LOGIN	,	EVERY_USER		,	2	,login_args		,	(void *)&login_user		},
	{REQURST_GET_ROOM	,REPLY_GET_ROOM	,	CONNECTED_USER	,	0	,NULL			,	(void *)&getRooms_user	},
	{REQUEST_JOIN_ROOM	,REPLY_JOIN_ROOM,	CONNECTED_USER	, 	1	,join_room_args ,	(void *)&joinRoom_user	},
	{REQUEST_SEND		,REPLY_SEND		,	IN_ROOM_USER	,	2	,send_args		,	(void *)&send_user		},
	{REQUEST_EXIT		,REPLY_EXIT		,	IN_ROOM_USER	,	1	,exit_args		,	(void *)&exit_user		}

    
};

user connected_users[MAX_CONNECTIONS];
int room_lock = 0;

int userHandle(void * arguments)
{
    struct args_s *args = (struct args_s *)arguments;
    int thread_id = args->thread_id;
    int sockfd = args->sockfd;
    printf("thread_id : %d\n",thread_id);
    connected_users[thread_id].sockfd = sockfd;
    connected_users[thread_id].access_level = EVERY_USER;

    message recv_msg,send_msg;
    request rq;

    int status = SUCCESS;
    char output[MAXDATASIZE] = "NO DATA";
    while(1)
    {
    memset(output,0,MAXDATASIZE);

    /* get the messege*/
    if(receive_message(sockfd,&recv_msg,MAXDATASIZE) == -2)
    {
        break;
    }

    if(recv_msg.opcode <100 || recv_msg.opcode >106 )
    {
        printf("invalid opcode\n");
        continue;
    }

    /*get arguments */

    rq = requests[recv_msg.opcode-100];
    if(rq.access > connected_users[thread_id].access_level)
    {
        printf("user access level is too low\n");
        continue;
    }
    char * args[2];
    char* token;
    char* rest = recv_msg.data;
    for (int i = 0; i < rq.argc; i++)
    {
        token = strtok_r(rest, "\t", &rest);
        args[i] = token;

    }
        
    /*Do something*/
    status = rq.operation(thread_id,output,args[0],args[1]);


    /* send the messge*/
    send_msg.opcode = rq.replay_code;
    send_msg.status = status;
    strcpy(send_msg.data,output);
    send_message(sockfd,&send_msg,MAXDATASIZE);


    }
    

}



/**
 * @brief add user to database
 * @param username new user's name
 * @param password new user's password
 * @return SUCCESS or FAILURE
 */
int register_user(int thread_id,char *output ,char * username , char * password)
{
    user new_user;
    if(username == NULL)
    {
        fprintf(stderr, "users: %s.\n",ERROR_MESSEGE_NAME_UNSPECIFIED);
        snprintf(output,MAXDATASIZE,"%s",ERROR_MESSEGE_NAME_EXISTS);
        return FAILURE;
    }
    if(findUserByUsername(username,&new_user) == SUCCESS)
    {
        fprintf(stderr, "users:%s.\n",ERROR_MESSEGE_NAME_EXISTS);
        snprintf(output,MAXDATASIZE,"%s",ERROR_MESSEGE_NAME_EXISTS);

        return FAILURE;
    }
    strncpy(new_user.username , username,BUF_SIZE);
    strncpy(new_user.password , password,PASSWORD_BUFFER_SIZE);

    if(writeUser(&new_user) == FAILURE)
    {
        return FAILURE;
    }

    /* update user */
    strncpy(connected_users[thread_id].username, username,BUF_SIZE);
    strncpy(connected_users[thread_id].password, password,BUF_SIZE);
    connected_users[thread_id].access_level = CONNECTED_USER;

    memset(output,0,MAXDATASIZE);     /* overwrite data */
    snprintf(output,MAXDATASIZE,"%s %s",username,password);
    
    return SUCCESS;
}



int login_user(int thread_id,char * output,char * username , char * password)
{
    user logging_user;
    if(username == NULL)
    {
        fprintf(stderr, "users: %s\n",ERROR_MESSEGE_NAME_UNSPECIFIED);
        snprintf(output,MAXDATASIZE,"%s",ERROR_MESSEGE_NAME_UNSPECIFIED);
        return FAILURE;
    }
    if(findUserByUsername(username,&logging_user) == FAILURE)
    {
        fprintf(stderr, "users: %s.\n",ERROR_MESSEGE_NAME_EXISTS);
        snprintf(output,MAXDATASIZE,"%s",ERROR_MESSEGE_NAME_EXISTS);
        return FAILURE;
    }
    else
    {
        printf("%s %s", password, logging_user.username);
        if(strncmp(logging_user.password, password, PASSWORD_BUFFER_SIZE)!=0)
        {
            fprintf(stderr,"users: %s\n",ERROR_MESSEGE_INCORRECT_PASSWORD);
            snprintf(output,MAXDATASIZE,"%s",ERROR_MESSEGE_INCORRECT_PASSWORD);

            return FAILURE;
        }

    }
    /* update user */
    strncpy(connected_users[thread_id].username, username,BUF_SIZE);
    strncpy(connected_users[thread_id].password, password,BUF_SIZE);
    connected_users[thread_id].access_level = CONNECTED_USER;

    memset(output,0,MAXDATASIZE);     /* overwrite data */
    snprintf(output,MAXDATASIZE,"%s %s",username,password);
    return SUCCESS;
    

}


int getRooms_user(int thread_id,char *output, void * arg1, void * arg2)
{
    room * rooms[ROOMS_AMOUNT];
    int i;
    
    getRooms(rooms);
    memset(output,0,MAXDATASIZE);     /* overwrite data */
    for (i = 0; i < ROOMS_AMOUNT &&rooms[i] != NULL; i++)
    {
        output = strncat(output,rooms[i]->room_name,BUF_SIZE);
        output = strncat(output," ",2);

    }
    
    return SUCCESS;// TODO : check status
    
}


int joinRoom_user(int thread_id,char * output, char * room_name)
{
    room r;
    FILE * room_file;
    if(getRoomByName(&r,room_name) == FAILURE)
    {
        return FAILURE;
    }
    room_file = fopen(r.room_file_path,"a");
    fprintf(room_file, "[SERVER] User \"%s\" has joined the room.\n",connected_users[thread_id].username);
    fclose(room_file);
    strncpy(connected_users[thread_id].room,room_name,BUF_SIZE);
    connected_users[thread_id].access_level = IN_ROOM_USER;
    
    return SUCCESS;
}


int send_user(int thread_id, char * output,char * msg)
{
    room r;
    FILE * room_file;
    if(getRoomByName(&r,connected_users[thread_id].room) == FAILURE)
    {
        return FAILURE;
    }
    room_file = fopen(r.room_file_path,"a");
    fprintf(room_file, "[%s] %s\n",connected_users[thread_id].username,msg);
    fclose(room_file);



    return SUCCESS;
}


int exit_user(int thread_id)
{
    room r;
    FILE * room_file;
    if(getRoomByName(&r,connected_users[thread_id].room) == FAILURE)
    {
        fprintf(stderr,"users: couldnt get room\n");
        return FAILURE;
    }
    room_file = fopen(r.room_file_path,"a");
    fprintf(room_file, "[SERVER] User \"%s\" has exited the room.\n",connected_users[thread_id].username);

    fclose(room_file);
    return SUCCESS;
}

int writeUser(user * new_user)
{
	FILE * file;
	file = fopen(USERS_FILE_PATH,"a");

	if(file == NULL)
	{
		fprintf(stderr, "\nError openeing a file\n");
        fclose(file);
        return FAILURE;
	}
    fflush(NULL);
    int flag =  fwrite(new_user->username,sizeof(char), strlen(new_user->username), file) ;
    fputc('\t',file);
    flag |= fwrite(new_user->password,sizeof(char), strlen(new_user->password), file);
    fputc('\n',file);

	if(flag)
	{
		printf("User \"%s %s\" saved!\n", new_user->username, new_user->password);
	}
	else
	{
		fprintf(stderr,"Error Writing to File\n");
	}
	fclose(file);
    return flag;
}

int findUserByUsername(char * username,user *saved_user)
{

	FILE * file;
    char * saved_username,saved_password;
	int is_found = FAILURE;
    char buf[BUF_SIZE+PASSWORD_BUFFER_SIZE+1];
    char *rest;
    char *tocket;
	file = fopen(USERS_FILE_PATH,"r");
	if(file == NULL)
	{
		fprintf(stderr, "\nError opened file\n");
        exit(1);
	}

	while(fgets(buf,BUF_SIZE,file))
	{
        buf[strcspn(buf, "\n")] = 0;

        rest = buf;
        tocket = strtok_r(rest,"\t ",&rest);
		if(strncmp(tocket, username,BUF_SIZE) == 0)
		{	
            strncpy(saved_user->username,username,BUF_SIZE);
            strncpy(saved_user->password,strtok_r(NULL,"\t ",&rest),PASSWORD_BUFFER_SIZE);
            printf("PASSWORD %s\n",saved_user->password);
			is_found = SUCCESS;
			printf("User: \"%s\" \n", saved_user->username);

			break;
		}
	}
	
	
	fclose(file);	
	return is_found;
}

void getRooms(room *rooms[ROOMS_AMOUNT]) {
    int room_count = 0;
    DIR *d;
    struct dirent *dir;
    char full_path[264];


    char *rest;
    char *base_name;
    char name_copy[FILE_NAME_BUFF_SIZE];
    d = opendir(ROOM_FOLDER_PATH);
    if (!d) {
        perror("Failed to open room folder");
        return;
    }

    while ((dir = readdir(d)) != NULL && room_count < ROOMS_AMOUNT) 
    {
        if (dir->d_type == DT_REG) 
        {

            snprintf(full_path, sizeof(full_path), "%s/%s", ROOM_FOLDER_PATH, (char*)dir->d_name);

            name_copy[FILE_NAME_BUFF_SIZE];
            strncpy(name_copy, dir->d_name, FILE_NAME_BUFF_SIZE - 1);
            name_copy[FILE_NAME_BUFF_SIZE - 1] = '\0';

            rest = NULL;
            base_name = strtok_r(name_copy, ".", &rest);

            rooms[room_count] = malloc(sizeof(room));

            strncpy(rooms[room_count]->room_file_path, full_path, FILE_NAME_BUFF_SIZE - 1);
            rooms[room_count]->room_file_path[FILE_NAME_BUFF_SIZE - 1] = '\0';

            strncpy(rooms[room_count]->room_name, base_name, FILE_NAME_BUFF_SIZE - 1);
            rooms[room_count]->room_name[FILE_NAME_BUFF_SIZE - 1] = '\0';

            printf("Room: %s (Path: %s)\n", rooms[room_count]->room_name, rooms[room_count]->room_file_path);
            room_count++;
        }
    }

    closedir(d);
}

int getRoomByName(room * room_output,char * room_name)
{
    DIR *d;
    FILE * room_file;
    struct dirent *dir;
    char line[BUF_SIZE];
    char * rest;
    char *file_name;
    char room_folder[32] = ROOM_FOLDER_PATH;
    int status = FAILURE;
    
    char * folder_path = strncat(room_folder,"/",2);
    d = opendir("./ROOMS");
    if (d) {
        while ((dir = readdir(d)) != NULL && status == FAILURE)
        {
            if(dir->d_type == DT_REG)   
            {
                rest = dir->d_name;
                file_name = strtok_r( rest,".",&rest);
                if(strncmp(room_name,file_name,FILE_NAME_BUFF_SIZE) == 0){
                    
                    folder_path = strncat(folder_path,dir->d_name,FILE_NAME_BUFF_SIZE);
                    folder_path = strncat(folder_path,".",2);
                    folder_path = strncat(folder_path,rest,FILE_NAME_BUFF_SIZE);

                    strncpy(room_output->room_name,file_name,FILE_NAME_BUFF_SIZE);
                    strncpy(room_output->room_file_path,folder_path,24);
                    
                    room_file = fopen(room_output->room_file_path,"r+");

                    fclose(room_file);

                    status = SUCCESS;

                }
               
            }
        
        }       
        closedir(d);
    }
    return status;
    


}
int multicast_messege(int thread_id,char * msg_data,int msg_length)
{
    user * current_user = &connected_users[thread_id];
    message send_msg;
    send_msg.opcode = REPLY_UPDATE;
    send_msg.status = SUCCESS;
    snprintf(send_msg.data, MAXDATASIZE,"[%s] %s",current_user->username,msg_data);


    while(room_lock == 1)
    {
        sleep(100);
    }
    
    
    for (int i = 0; i < MAX_CONNECTIONS; i++)
    {
        if(thread_id != i && strncmp(current_user->room,connected_users[i].room,BUF_SIZE) == 0)
        {
            send_message(connected_users->sockfd,&send_msg,MAXDATASIZE);
        }
    }
    room_lock = 0;
    

}