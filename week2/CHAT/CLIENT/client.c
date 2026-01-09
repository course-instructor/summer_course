#define _XOPEN_SOURCE 600
#include <stdio.h>
#include "client.h"
#define PORT "3490" 
#define MAX_CONNECTIONS 50
#define DEBUG 0



int main(int argc, char *argv[])
{

	
	/*write "localhost" if you host sever localy*/

	clientSide(argc,argv); 

	return SUCCESS;
}
/**
 * @brief tries to establish connection with the server
 */
int clientSide(int argc, char *argv[])
{
	int retval = SUCCESS;
	int sockfd = -1;
	if(argc != 2)
	{
		printf("Server address is not specified!\n");
		fprintf(stderr,"client hostname\n");
	    retval = FAILURE;
	}

	if(establish_connection(argv[1],&sockfd)== FAILURE)
	{
		fprintf(stderr,"Could not establish connection\n");
		retval = FAILURE;

	}
	else
	{
		pthread_t input_thread;
		printf("%d\n",sockfd);
		pthread_create(&input_thread,NULL,(void*)userInterface,&sockfd);
		pthread_join(input_thread,NULL);

	}


	return retval;
	
}
/**
 * @brief send message to socket
 */
int send_message(int sockfd, const message *msg, size_t data_size) 
{
	int retval = SUCCESS;
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
        retval = FAILURE;
    }
	#if DEBUG
		printf("SEND MSG : %d %d %s\n",msg->opcode,msg->status,msg->data);
	#endif

	return retval;
}

/**
 * @brief receive messege from sockets
 */
int receive_message(int sockfd, message *msg) 
{
	int status = SUCCESS;
	char buf[sizeof(message)];
	int num_bytes = 0;
	int offset = 0;

	memset(buf,0,sizeof(message));

	if((num_bytes = recv(sockfd,buf,sizeof(message),0)) == -1)
	{
		perror("recv");
		status = FAILURE;
	}
	else if(num_bytes == 0)
	{
		printf("disconnected!\n");
		status = DISCONNECT;
	}

	if(status == SUCCESS)
	{
		memcpy(&msg->opcode,buf+offset,sizeof(msg->opcode));
		offset+=sizeof(msg->opcode);
		memcpy(&msg->status,buf+offset,sizeof(msg->status));
		offset+=sizeof(msg->status);
		memcpy(msg->data,buf+offset,num_bytes-offset);

	#if DEBUG
		printf("RECV MSG : %d %d %s\n",msg->opcode,msg->status,msg->data);
	#endif

	}


	return status;
}

/**
 * @brief send new request
 */
int sendRequest(request * rq, message * send_msg,message * recv_msg,int sockfd)
{
	int data_size = 0;
	int status;
	int i;
	char args[2][BUF_SIZE] = {0};
	char output[MAXDATASIZE];
	recv_msg->status = SUCCESS; /*defult status*/
	for (i = 0; i < rq->argc; i++)
	{
		
		data_size+= rq->args_size[i];
	}
	/* do the local logical operation that corresponds to the obtained request*/
	status =rq->operation(output,args[0],args[1]);
	if(rq->request_code == REQUEST_SEND && strncmp(output,EXIT_COMMAND,MAXDATASIZE) == 0)
	{
		send_msg->opcode = REQUEST_EXIT;
		
	}
	else
	{
		send_msg->opcode = rq->request_code;

	}


	send_msg->status = status;

	memcpy(send_msg->data,output,data_size);
	
	status = FAILURE;
	/* send request */
	if(send_message(sockfd,send_msg,data_size)==FAILURE)
	{
		fprintf(stderr,"couldnt send the messege\n");
	}
	/*recive replay */
	if(send_msg->opcode != REQUEST_SEND && send_msg->opcode != REQUEST_EXIT)
	{
		status = receive_message(sockfd,recv_msg);
		if(status==FAILURE)
		{	
			fprintf(stderr,"couldnt recive the messege\n");
		
		}
	}
	else
	{
		

		if(recv_msg->opcode == REPLY_UPDATE)
		{
			printf("%s\n",recv_msg->data);
		}
		#if DEBUG
			printf("RECV MSG %d %d %s\n",recv_msg->opcode , recv_msg->status , recv_msg->data);
		#endif
		status = SUCCESS;
	
		
	}
	
	return status;

	

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
int establish_connection(char * address,int *sockfd)
{
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN]; /* size of IPV6 adress*/



	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(address, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return FAILURE;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((*sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(*sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("client: connect");
			close(*sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return FAILURE;
	}
	/*recive welcome messeg */



	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	recv(*sockfd,buf,MAXDATASIZE,0);
	printf("%s\n",buf);

	return SUCCESS;
}
