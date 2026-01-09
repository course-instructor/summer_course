#define _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE
#define __FAVOR_BSD

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <stddef.h>
#include <termios.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include "../HEADER/my_sniffer.h"
/* structs to enable/disable raw mode */
struct termios raw;
struct termios original;

char* filepath_packets_data = "/home/alon/Desktop/summer_army_course/week4/my_sniffer/packetsdata.txt";
char* filepath_info_packet = "/home/alon/Desktop/summer_army_course/week4/my_sniffer/packetsinfo.txt";
int packet_id_g = 1;
char buffer_g[DATA_SIZE_g];
int buflen = 0;
int run = OFF;
int thread_created = OFF;
int original_saved = OFF;
int able_to_press_s = ON;

int my_sniffer_open_socket()
{
    int sock_r;
    sock_r=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(sock_r<0)
    {
        printf("error while opening socket\n");
    }
    
    return sock_r;

}

int my_sniffer_recieve(int sock_r)
{
    memset(buffer_g,0,DATA_SIZE_g);
    struct sockaddr saddr;
    int saddr_len = sizeof (saddr);
    buflen=recvfrom(sock_r,buffer_g,DATA_SIZE_g,0,&saddr,(socklen_t *)&saddr_len);
    if(buflen<0)
    {
        printf("error in recieving\n");
    }
    return buflen;

}

int my_sniffer_print_ethheader(FILE *file_packet_log)
{
    int retval = SUCCESS;
    if(!file_packet_log)
    {
        perror("Couldn't open file\n");
        retval = FAIL;
    }
    if(retval)
    {
        struct  ethhdr *eth_head = (struct ethhdr *)(buffer_g);
        fprintf(file_packet_log,"\nEthernet Header\n");
        fprintf(file_packet_log,"\t |-Source Address : %02X:%02X:%02X:%02X:%02X:%02X\n",eth_head->h_source[0], eth_head->h_source[1], eth_head->h_source[2],
            eth_head->h_source[3], eth_head->h_source[4], eth_head->h_source[5]);

        fprintf(file_packet_log,"\t |-Destination Address : %02X:%02X:%02X:%02X:%02X:%02X\n" ,eth_head->h_dest[0],eth_head->h_dest[1],eth_head->h_dest[2],
            eth_head->h_dest[3], eth_head->h_dest[4], eth_head->h_dest[5]);

        fprintf(file_packet_log,"\t |-Protocol : %02X\n" , eth_head->h_proto);
        
    }
    return retval;

}

int my_sniffer_print_ipheader(FILE *file_packet_log,data_packet *packet_info_to_print)
{
    int retval = SUCCESS;
    if(!file_packet_log)
    {
        perror("File is closed\n");
        retval = FAIL;
    }
    if(retval)
    {
        struct  ethhdr *eth_head = (struct ethhdr *)(buffer_g);
        if(eth_head->h_proto == htons(ETH_P_IP))
        {
            struct iphdr *ip_head = (struct iphdr *)(buffer_g + sizeof(struct ethhdr));
            fprintf(file_packet_log,"IP Header\n");
            fprintf(file_packet_log,"\t |-Version : %d\n", (int)ip_head->version);
            fprintf(file_packet_log,"\t |-Header Length : %d Bytes\n", (int)ip_head->ihl * 4);
            fprintf(file_packet_log,"\t |-Type of Service : %d\n", (int)ip_head->tos);
            fprintf(file_packet_log,"\t |-Total Length : %d Bytes\n", ntohs(ip_head->tot_len));
            fprintf(file_packet_log,"\t |-Identification : %d\n", ip_head->id);
            fprintf(file_packet_log,"\t |- Time To Live : %d\n", (int)ip_head->ttl);
            fprintf(file_packet_log,"\t |-Protocol : %d\n", (int)ip_head->protocol);
            fprintf(file_packet_log,"\t |-Header Checksum : %d\n", ip_head->check);
            fprintf(file_packet_log,"\t |-Source Address : %s\n", inet_ntoa(*(struct in_addr *)&ip_head->saddr));
            strncpy(packet_info_to_print->source_ip,inet_ntoa(*(struct in_addr *)&ip_head->saddr),IP_LEN_G);
            fprintf(file_packet_log,"\t |-Destination Address : %s\n", inet_ntoa(*(struct in_addr *)&ip_head->daddr));
            strncpy(packet_info_to_print->destination_ip,inet_ntoa(*(struct in_addr *)&ip_head->daddr),IP_LEN_G);
        }
        else
        {
            fprintf(file_packet_log,"Unknown Protocol: %02X\n", ntohs(eth_head->h_proto));
            retval = FAIL;
        }
    }
    return retval;
}

int my_sniffer_tcp_or_udp(FILE *file_packet_log, data_packet *packet_info_to_print)
{
    int retval = SUCCESS;
    if(!file_packet_log)
    {
        perror("File is closed\n");
        retval = FAIL;
    }
    if(retval)
    {
        struct iphdr *ip_head = (struct iphdr *)(buffer_g + sizeof(struct ethhdr));
        int ipheader_length = ip_head->ihl * 4;
        int protocol = (int)ip_head->protocol;
        packet_info_to_print->starting_location_in_file = ftell(file_packet_log);
        printf("PROTCOL num is: %d\n",protocol);
        if(protocol == TCP_NUM) 
        {
            strncpy(packet_info_to_print->protocol,"TCP", PROTCOL_LEN_G);
            fprintf(file_packet_log,"*********************TCP Packet***********************\n");
        }
        else if(protocol == UDP_NUM) 
        {
            strncpy(packet_info_to_print->protocol,"UDP", PROTCOL_LEN_G);
            fprintf(file_packet_log,"*********************UDP Packet***********************\n");
        }
        else if(protocol == ICMP_NUM) 
        {
            strncpy(packet_info_to_print->protocol,"ICMP", PROTCOL_LEN_G);
            fprintf(file_packet_log,"*********************ICMP Packet***********************\n");
        }
        else 
        {
            fprintf(file_packet_log,"Unknown Protocol: %d\n", protocol);
        }
        retval = protocol;
    }
    return retval;
}

int my_sniffer_print_transport_header(FILE* file_packet_log, data_packet* packet_info_to_print )
{
    int retval = SUCCESS;
    if(!file_packet_log)
    {
        perror("File is closed\n");
        retval = FAIL;
    }
    if(retval)
    {
        struct iphdr *ip_head = (struct iphdr *)(buffer_g + sizeof(struct ethhdr));
        int ipheader_length = ip_head->ihl * 4;
        int protocol = (int)ip_head->protocol;
        if(protocol == TCP_NUM) 
        {
            struct tcphdr *tcp_head = (struct tcphdr *)(buffer_g + sizeof(struct ethhdr) + ipheader_length);
            fprintf(file_packet_log,"TCP Header\n");
            fprintf(file_packet_log,"\t |-Source Port : %d\n", tcp_head->source);
            packet_info_to_print->src_port = tcp_head->source;
            fprintf(file_packet_log,"\t |-Destination Port : %d\n", tcp_head->dest);  
            packet_info_to_print->dest_port = tcp_head->dest;
            fprintf(file_packet_log,"\t |-Sequence Number : %u\n", tcp_head->seq);
            fprintf(file_packet_log,"\t |-Acknowledgment Number : %u\n", tcp_head->ack_seq);
            fprintf(file_packet_log,"\t |-Header Length : %d Bytes\n", tcp_head->doff * 4);
            fprintf(file_packet_log,"\t |---------------Flags--------------\n");
            fprintf(file_packet_log,"\t\t |-Urgent Flag : %d\n", (int)tcp_head->urg);
            fprintf(file_packet_log,"\t\t |-Acknowledgment Flag : %d\n", (int)tcp_head->ack);
            fprintf(file_packet_log,"\t\t |-Push Flag : %d\n", (int)tcp_head->psh);
            fprintf(file_packet_log,"\t\t |-Reset Flag : %d\n", (int)tcp_head->rst);
            fprintf(file_packet_log,"\t\t |-Synchronize Flag : %d\n", (int)tcp_head->syn);
            fprintf(file_packet_log,"\t\t |-Finish Flag : %d\n", (int)tcp_head->fin);
            fprintf(file_packet_log,"\t |-Window Size : %d\n", (tcp_head->window));
            fprintf(file_packet_log,"\t |-Checksum : %d\n", tcp_head->check);
            fprintf(file_packet_log,"\t |-Urgent Pointer : %d\n", tcp_head->urg_ptr);
        }
        else if(protocol == UDP_NUM) 
        {
            struct udphdr *udp_head = (struct udphdr *)(buffer_g + sizeof(struct ethhdr) + ipheader_length);
            fprintf(file_packet_log,"UDP Header\n");
            fprintf(file_packet_log,"\t |-Source Port : %d\n", ntohs(udp_head->source));
            packet_info_to_print->src_port = ntohs(udp_head->source);
            fprintf(file_packet_log,"\t |-Destination Port : %d\n", ntohs(udp_head->dest));
            packet_info_to_print->dest_port = ntohs(udp_head->dest);
            fprintf(file_packet_log,"\t |-UDP Length : %d Bytes\n", ntohs(udp_head->len));
            fprintf(file_packet_log,"\t |-UDP Checksum : %d\n", udp_head->check);
        }
        else if(protocol == ICMP_NUM) 
        {
            struct icmphdr *icmp_head = (struct icmphdr *)(buffer_g + sizeof(struct ethhdr) + ipheader_length);
            fprintf(file_packet_log,"ICMP Header\n");
            fprintf(file_packet_log,"\t |-Type : %d\n", icmp_head->type);
            fprintf(file_packet_log,"\t |-Code : %d\n", icmp_head->code);
            fprintf(file_packet_log,"\t |-Checksum : %d\n", icmp_head->checksum);
            fprintf(file_packet_log,"\t |-Identifier : %d\n", ntohs(icmp_head->un.echo.id));
            fprintf(file_packet_log,"\t |-Sequence Number : %d\n", ntohs(icmp_head->un.echo.sequence));
            strncpy(packet_info_to_print->protocol,"ICMP", PROTCOL_LEN_G);
        }
        else 
        {
            printf("Unknown Protocol: %d\n", protocol);
        }
        retval = protocol;
    }
    return retval;
}

int my_sniffer_print_data(FILE * file_packet_log, data_packet* packet_info_to_print)
{
    int retval = SUCCESS;
    if(!file_packet_log)
    {
        perror("File is closed\n");
        retval = FAIL;
    }
    if(retval)
    {
        int protocol = my_sniffer_print_transport_header(file_packet_log,packet_info_to_print);
        struct iphdr *ip_head = (struct iphdr *)(buffer_g + sizeof(struct ethhdr));
        int ipheader_length = ip_head->ihl * 4;
        if(protocol == TCP_NUM)
        {
            
            struct tcphdr *tcp_head = (struct tcphdr *)(buffer_g + sizeof(struct ethhdr) + ipheader_length);
            char *data = (char*)buffer_g + sizeof(struct ethhdr) + ipheader_length +tcp_head->doff * 4;
            int len_data = ntohs(ip_head->tot_len) - (ipheader_length + tcp_head->doff * 4);
            if(len_data < 0 || buflen < (sizeof(struct ethhdr) + ipheader_length + tcp_head->doff*4 + len_data) && retval)
            {
                fprintf(file_packet_log,"No data to print for this protocol\n");
                retval = FAIL;
            }
            if(retval)
            {
                fprintf(file_packet_log,"Data\n");
                int written;
                for(written = 0; written < len_data; written++)
                {
                    if(written % 16 == 0)
                    {
                        fprintf(file_packet_log,"\n");
                    }
                    fprintf(file_packet_log," %02X ", (unsigned char)*(data + written));
                }
                fprintf(file_packet_log,"\n");
                fprintf(file_packet_log,"**************************************************************\n");
            }
        }
        else if(protocol == UDP_NUM)
        {
            fprintf(file_packet_log,"Data\n");
            struct udphdr *udp_head = (struct udphdr*)(buffer_g + sizeof(struct ethhdr) +ipheader_length);
            char *data = (char*) buffer_g + sizeof(struct ethhdr) + ipheader_length + sizeof(struct udphdr);
            int len_data = ntohs(udp_head->len) - sizeof(struct udphdr);
            int written;
            for(written = 0; written < len_data; written++)
            {
                if(written % 16 == 0)
                {
                    fprintf(file_packet_log,"\n");
                }
                fprintf(file_packet_log,"%02X ", (unsigned char)*(data + written));
            }
            fprintf(file_packet_log,"\n");
            fprintf(file_packet_log,"**************************************************************\n");
        }
        else if(protocol == ICMP_NUM)
        {
            fprintf(file_packet_log,"Data\n");
            
            fprintf(file_packet_log,"**************************************************************\n");
        }
        else
        {
            fprintf(file_packet_log,"No data to print for this protocol\n");
        }
    }
    packet_info_to_print->end_location_in_file = ftell(file_packet_log);
    packet_info_to_print->num_lines = packet_info_to_print->end_location_in_file - packet_info_to_print->starting_location_in_file;
    return retval;
}

int packet_runner()
{
    data_packet print_info;
    FILE *packet_file = fopen(filepath_packets_data,"w");
    int retval = SUCCESS;
    FILE *file_packet_info_ptr = fopen(filepath_info_packet,"w");
    if(!file_packet_info_ptr)
    {
        perror("Culdn't open info file\n");
        retval = FAIL;
    }
    if(retval)
    {
        if(!packet_file)
        {
            perror("Couldn't open file\n");
            retval = FAIL;
        }
    }
    int sockfd = my_sniffer_open_socket();
    while(run && retval)
    {
        print_info.id = packet_id_g++;
        my_sniffer_recieve(sockfd);
        my_sniffer_tcp_or_udp(packet_file,&print_info);
        my_sniffer_print_ethheader(packet_file);
        my_sniffer_print_ipheader(packet_file,&print_info);
        my_sniffer_print_data(packet_file,&print_info);
        printf("%d %s packet from (%s,%d) to (%s,%d)\n",print_info.id,print_info.protocol,print_info.source_ip,print_info.src_port,
                                                        print_info.destination_ip,print_info.dest_port);
        fflush(file_packet_info_ptr);
        fprintf(file_packet_info_ptr,"%d,%ld,%ld\n",print_info.id,print_info.starting_location_in_file,print_info.num_lines);
    }
    packet_id_g = 0;
    fclose(packet_file);
    fclose(file_packet_info_ptr);
    close(sockfd);
    return 0;
}

int copy_requested_packet(int line_id)
{
    char *packet_info_loaction = malloc(100);
    char *byte_start_id;
    char *byte_end_id;
    int retval = SUCCESS;
    FILE* file_packets_info = fopen(filepath_info_packet,"r");
    if(!file_packets_info)
    {
        perror("Couldn't open info file\n");
        retval = FAIL;
    }
    if(retval)
    {
        FILE *file_data_packets = fopen(filepath_packets_data,"r");
        if(!file_data_packets)
        {
            perror("Couldn't open data file\n");
            retval = FAIL;
        }
        if(retval)
        {
            int line_num;
            /* we skip to the line we need */
            for(line_num = 1; line_num < line_id; line_num++)
            {
                if(!fgets(packet_info_loaction,100,file_packets_info))
                {
                    packet_info_loaction[0] = '\0';
                }
            }
            fgets(packet_info_loaction,100,file_packets_info);
            strtok(packet_info_loaction,",");
            byte_start_id = strtok(NULL , ",");
            byte_end_id = strtok(NULL , ",");
            char *endptr;
            /* we get here the offset of the start of the data*/
            int starting_byte_of_data = (int)strtol(byte_start_id,&endptr,10);
            /* we get here the amount of bytes (length) of our packet */
            int data_len = (int)strtol(byte_end_id,&endptr,10);
            fseek(file_data_packets,starting_byte_of_data,SEEK_SET);
            fread(buffer_g,1,data_len,file_data_packets);
            buffer_g[data_len] = '\0';
            /* we print the packet */
            printf("%s\n",buffer_g);
            fclose(file_data_packets);
        }
            fclose(file_packets_info);
    }
    return retval;
}

void disableRAWMode() 
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}


void enableRAWMode() 
{
    struct termios raw;

    if (!original_saved) 
    {
        tcgetattr(STDIN_FILENO, &original); 
        original_saved = ON;
        atexit(disableRAWMode); 
    }
    raw = original;
    raw.c_lflag &= ~(ECHO | ICANON); 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); 
}

void *read_stdin(void *arg) 
{
    enableRAWMode();
    while(1)
    {
        char key_pressed;
        key_pressed = getchar();
        if(key_pressed == 's')
        {
            if(!able_to_press_s)
            {
                printf("You can't press 's' now, please press ESC\n");
            }
            else
            {
                if(!thread_created && !run)
                {
                    pthread_t thread_id;
                    pthread_create(&thread_id, NULL, (void *)packet_runner, NULL);
                    thread_created = ON;
                    run = ON;
                    pthread_detach(thread_id);
                }
                else
                {
                    printf("Sniffer is already running\n");
                }
            }
        }
        else if(key_pressed == 'k')
        {
            if(!run)
            {
                printf("Sniffer is not running\n");
            }
            else
            {
                run = OFF;
                printf("Sniffer stopped\n");
                thread_created = OFF;

            }
        }
        else if(key_pressed == 'i')
        {
            able_to_press_s = OFF;
            int line_id;
            disableRAWMode();
            tcflush(STDIN_FILENO, TCIFLUSH);
            printf("enter the id packet that you want to see\n");
            scanf("%d", &line_id);
            if(line_id <= 0)
            {
                printf("Invalid packet ID\n");  
            }
            else
            {
                copy_requested_packet(line_id);
            }
            enableRAWMode();
        }
        else if(key_pressed == 27)
        {
            able_to_press_s = ON;
        }
        // else if(key_pressed == 'd')
        // {
        //     char file_path_d[100];
        //     file_path_d[0] = '\0';
        //     strcpy(file_path_d,"my_sniffer_");
        //     time_t current_time = time(NULL);
        //     struct tm *date_tm = localtime(&current_time);
        //     date_tm->tm_year += 1900; 
            
            
        // }
    }
    return NULL;

}

