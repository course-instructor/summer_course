/*
** server.c -- a stream socket server demo
*/
#define _XOPEN_SOURCE 600
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
#include <pthread.h>
#include <arpa/inet.h> 
#include "server.h"
#define MAX_CONNECTIONS 50
#define PORT "3490" 
#define WELCOME_MASSAGE "Welcome to my message up!"
#define AMOUNT_OF_REQUESTS 6
#define USERNAME_SIZE USERNAME_BUFFER_SIZE*sizeof(char)
#define PASSWORD_SIZE PASSWORD_BUFFER_SIZE*sizeof(char)
#define ROOM_SIZE FILE_NAME_BUFF_SIZE*sizeof(char)
#define MSG_SIZE BUF_SIZE*sizeof(char)




int receive_message(int sockfd, message *msg) 
{
	
	char buf[sizeof(message)];
	int num_bytes = 0;
	int offset = 0;
	if((num_bytes = recv(sockfd,buf,sizeof(message),0)) == -1)
	{
		perror("recv");
		return FAILURE;
	}
	if(num_bytes == 0)
	{
		printf("disconnected!\n");
		return DISCONNECT;
	}


	memcpy(&msg->opcode,buf+offset,sizeof(msg->opcode));
	offset+=sizeof(msg->opcode);
	memcpy(&msg->status,buf+offset,sizeof(msg->status));
	offset+=sizeof(msg->status);
	memcpy(msg->data,buf+offset,num_bytes-offset);

	printf("RECV MSG : %d %d %s\n",msg->opcode,msg->status,msg->data);

	return SUCCESS;
}

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
    if (send(sockfd, buf, MAXDATASIZE, 0) == -1) {
        perror("send opcode");
        return FAILURE;
    }
	printf("SEND MSG : %d %d %s\n",msg->opcode,msg->status,msg->data);

	return SUCCESS;
}

// get sockaddr, IPv4 or IPv6:
void *getInAddr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) 
	{
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/**
 * @brief initializes servinfo and prepares for binding
 * @param servinfo 
 */
void getAddress(struct addrinfo **servinfo,char * port)
{
    int status;
    struct addrinfo hints;          /*servers adress*/


    memset(&hints,0,sizeof hints);  /*make sure struct is empty*/ 
    hints.ai_family = AF_UNSPEC;    /*IPv4 or IPv6*/
    hints.ai_socktype = SOCK_STREAM;/*TCP steam scokets*/
    hints.ai_flags = AI_PASSIVE;    /*fill in with my local ip*/

    /*Check if address retrived successfully */
    if( ( status = getaddrinfo(NULL, port, &hints, servinfo) ) != 0)
    {
        fprintf(stderr,"getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    
}
/**
 * @brief initializes sockets and binds them
 * @param servinfo adress
 * @param sockfd pointer to file descriptor
 */
void CreateAndBindSockets(struct addrinfo *servinfo, int * pSockfd)
{
    int sockfd; /*file descriptor - file in which the data will be written and read from*/
    int yes=1;
    struct addrinfo *p;

    /* loop through all the results and bind to the first we can */
    for(p = servinfo; p != NULL; p = p->ai_next) 
	{
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) 
		{
			perror("server: socket");
			continue;
		}
        /* for reusing the same adress*/
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(yes)) == -1) 
		{
			perror("setsockopt");
			exit(1);
		}
        /* bind socket - associate socket with a port on local machine*/
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}
	freeaddrinfo(servinfo); /*all done with this structure*/ 
    if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
    *pSockfd = sockfd;      /*pass file descriptor*/


}
void newUserThread(int * threads_flags_is_running,int sockfd,struct args_s thread_arguments)
{
	pthread_t thread;
	

	thread_arguments.sockfd = sockfd;
	thread_arguments.thread_id = -1;

	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		if(threads_flags_is_running[i] == 0)
		{
			threads_flags_is_running[i] = 1;
			thread_arguments.thread_id = i;
			thread_arguments.is_running = &threads_flags_is_running[i];
			printf("ttthread : %d \n",thread_arguments.thread_id );
			break;
		}
	}
	if(thread_arguments.thread_id == -1)
	{
		printf("connections full. connection declined.\n");
		send(sockfd,"connections full. sorry!",MAXDATASIZE,0);

		close(sockfd);
	}
	else
	{
		send(sockfd,WELCOME_MASSAGE,MAXDATASIZE,0);
		pthread_create(&thread, NULL, (void*)userHandle,(void*)&thread_arguments);
		pthread_detach(thread);
	}

}

void socketListener(int sockfd)
{
    int new_fd;
    socklen_t sin_size;
	struct args_s thread_arguments;
    struct sockaddr_storage their_addr; /*connector's address information*/ 
    char s[INET6_ADDRSTRLEN] /*ipv6 size , becuse it could be ipv4 and ipv6 so to take into account them both I use the bigger size*/;
	int threads_id[MAX_CONNECTIONS] = {0};

    /* lisen to incoming messages */
    if (listen(sockfd, MAX_CONNECTIONS) == -1) {
		perror("listen");
		exit(1);
	}
    while(1) 
	{  /* main accept() loop */ 
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			continue;
		}

		inet_ntop(their_addr.ss_family,
			getInAddr((struct sockaddr *)&their_addr),
			s, sizeof s);
		printf("server: got connection from %s\n", s);

		newUserThread(threads_id,new_fd,thread_arguments);
		
	}
		close(new_fd);
		exit(0);
}



int main(void)
{
    struct addrinfo *res;
    int sockfd;
    printf("Retriving address...\n");
    getAddress(&res, PORT);
    printf("Binding sockets...\n");
    CreateAndBindSockets(res,&sockfd);
    printf("Listening for messages...\n");
    socketListener(sockfd);

	

	return 0;
}