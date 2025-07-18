#define _XOPEN_SOURCE 600
#include <stdio.h>
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
#include "client.h"
#define PORT "3490" 
#define MAX_CONNECTIONS 50




int main(int argc, char *argv[])
{
	/*write "localhost" if you host sever localy*/
	clientSide(argc,argv); 

	
}
/**
 * @brief start program
 */
void clientSide(int argc, char *argv[])
{
	int sockfd = -1;
	if(argc != 2)
	{
		fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	if((sockfd = establish_connection(argv[1]))== FAILURE)
	{
		printf("Could not establish connection\n");
		exit(1);
	}
	pthread_t input_thread;
	pthread_create(&input_thread,NULL,(void*)userInterface,&sockfd);
	pthread_join(input_thread,NULL);

}
/**
 * @brief send message to socket
 */
int send_message(int sockfd, const message *msg, size_t data_size) 
{
	char buf[sizeof(message)];
	int offset = 0;
	memcpy(buf+offset,&msg->opcode,sizeof(msg->opcode));
	offset+=sizeof(msg->opcode);
	memcpy(buf+offset,&msg->status,sizeof(msg->status));
	offset+=sizeof(msg->status);
	memcpy(buf+offset,msg->data,data_size);
	offset+=data_size;

	

    /* Send opcode */
    if (send(sockfd, buf, sizeof(message), 0) == -1) {
        perror("send opcode");
        return FAILURE;
    }
	printf("SEND MSG : %d %d %s\n",msg->opcode,msg->status,msg->data);
	return SUCCESS;
}

/**
 * @brief receive messege from sockets
 */
int receive_message(int sockfd, message *msg, size_t data_size) 
{
	char buf[sizeof(message)];
	memset(buf,0,sizeof(message));
	int num_bytes = 0;
	int offset = 0;
	memset(buf,0,sizeof(message));

	if((num_bytes = recv(sockfd,buf,sizeof(message),0)) == -1)
	{
		perror("recv");
		return FAILURE;
	}
	if(num_bytes == 0)
	{
		printf("disconnected!\n");
		return -2;
	}


	memcpy(&msg->opcode,buf+offset,sizeof(msg->opcode));
	offset+=sizeof(msg->opcode);
	memcpy(&msg->status,buf+offset,sizeof(msg->status));
	offset+=sizeof(msg->status);
	memcpy(msg->data,buf+offset,num_bytes-offset);

	printf("RECV MSG : %d %d %s\n",msg->opcode,msg->status,msg->data);

	return SUCCESS;
}

/**
 * @brief send new request
 */
int sendRequest(request * rq, message * send_msg,message * recv_msg,int sockfd)
{
	int data_size = 0;
	int status = 0;
	char args[2][BUF_SIZE] = {0};
	char output[MAXDATASIZE];
	recv_msg->status = SUCCESS; /*defult status*/
	for (size_t i = 0; i < rq->argc; i++)
	{
		
		data_size+= rq->args_size[i];
	}
	/* do something */
	status =rq->operation(output,args[0],args[1]);
	send_msg->opcode = rq->request_code;
	send_msg->status = status;
	memcpy(send_msg->data,output,data_size);


	if(send_message(sockfd,send_msg,data_size)==FAILURE)
	{
		printf("couldnt send the messege\n");
		return FAILURE;
	}


	if(send_msg->opcode != REQUEST_SEND && receive_message(sockfd,recv_msg,MAXDATASIZE)==FAILURE)
	{
		printf("couldnt recive the messege\n");
		return FAILURE;
	} 
	//printf("OPCODE: %d MSG: %s\n",recv_msg->opcode , recv_msg->data);
	return recv_msg->status;

	

}
/**
 * @brief thread for reading incoming messeges
 */
void recive_update(int *sockfd)
{
    message recv_msg;
    
    while(1)
    {
        if(receive_message(*sockfd,&recv_msg,MAXDATASIZE) < 0)
		{
			break;
		}
        if(recv_msg.opcode == REPLY_UPDATE)
        {
            printf("%s\n",recv_msg.data);
        }
        else if(recv_msg.opcode == REPLY_EXIT)
        {
			printf("[Exiting room...]");
            break;
        }
    }

}






void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
/**
 * @brief connect to server
 */
int establish_connection(char * address)
{
	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];



	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(address, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return FAILURE;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return FAILURE;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	return sockfd;
}
