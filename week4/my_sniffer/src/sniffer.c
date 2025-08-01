#define _GNU_SOURCE
#include <stdio.h>
#include "my_sniffer.h"

extern uint64_t packet_count;
extern bool is_sniffing;

status sniffer_start()
{
    int sock_fd;
    packet_count = 0;
    status return_status = SUCCESS;


    FILE * tmp_file = NULL;
    FILE * offset_file = NULL;

    if((tmp_file = fopen(TEMPORARY_FILE_PATH,"wb")) == NULL)
    {
        perror("Error opening file");
        return_status = FAILURE;
    }
    else if((offset_file = fopen(OFFSET_FILE_PATH,"wb")) == NULL)
    {
        perror("Error opening file");
        return_status = FAILURE;
    }
    else if(sniffer_create_socket(&sock_fd) == FAILURE)
    {
        return_status = FAILURE;
    }
    else
    {
        while (return_status == SUCCESS && is_sniffing == true)
        {
            return_status = sniffer_packet_reader(&sock_fd,tmp_file,offset_file);

        }
        

    }
    if(tmp_file != NULL)
    {
        fclose(tmp_file);

    }
    else if(offset_file != NULL)
    {
        fclose(offset_file);
    }

    close(sock_fd);

    return return_status;
}


status sniffer_create_socket(int * sock_fd)
{
    status return_status = SUCCESS;

    *sock_fd=socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));
    if(*sock_fd<0)
    {
        fprintf(stderr,"error in sockets\n");
        return_status = FAILURE;    
    }

    return return_status;
}

status sniffer_packet_reception(int sock_fd, uint8_t *buffer, ssize_t * buflen)
{
    status return_status = SUCCESS;
    struct sockaddr saddr;

    int saddr_len = sizeof (saddr);

    memset(buffer,0,MAX_DATA_SIZE);


    /*Receive a network packet and copy in to buffer*/
    
    *buflen = recvfrom(sock_fd,buffer,MAX_DATA_SIZE,0,&saddr,(socklen_t *)&saddr_len);
    if(*buflen < 0)
        {
        fprintf(stderr,"Error in reading from recvform function");
        return_status = FAILURE;
        }


    return return_status;
}



status sniffer_packet_reader(int * sock_fd, FILE * tmp_file , FILE * offset_file)
{
    uint8_t buffer[MAX_DATA_SIZE];
    ssize_t buflen = 0;
    uint64_t current_id;
    status return_status = SUCCESS;
 
    if (sniffer_packet_reception(*sock_fd,buffer,&buflen) == FAILURE)
    {
        return_status = FAILURE;
    }
    else
    {
        current_id = sniffer_save_raw_packet(buffer,buflen, tmp_file, offset_file);
        sniffer_extract_summery(buffer,current_id);
        packet_count++; /*increment global packet count*/
    }
    return return_status;

}

void sniffer_exract_ethernet_header(uint8_t * buffer, FILE * output)
{
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    fprintf(output , "\n******************** Ethernet Header  ********************\n");

    fprintf(output , "\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    fprintf(output , "\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    fprintf(output , "\t|-Protocol : %d\n",eth->h_proto);
    
}
void sniffer_extract_ip_header(uint8_t * buffer, u_int16_t * iphdrlen, FILE * output)
{

    struct sockaddr_in source = {0}, dest = {0};
    struct iphdr *ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));


    /* getting actual size of IP header*/
    *iphdrlen = (u_int16_t)ip->ihl*4;

    memset(&source, 0, sizeof(source));
    source.sin_family = AF_INET;
    source.sin_addr.s_addr = ip->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = ip->daddr;

    fprintf(output , "\n********************    IP Header     ********************\n");

    fprintf(output , "\t|-Version : %d\n",(unsigned int)ip->version);
    fprintf(output , "\t|-Internet Header Length : %d DWORDS or %d Bytes\n",(unsigned int)ip->ihl,((unsigned int)(ip->ihl))*4);
    fprintf(output , "\t|-Type Of Service : %d\n",(unsigned int)ip->tos);
    fprintf(output , "\t|-Total Length : %d Bytes\n",ntohs(ip->tot_len));
    fprintf(output , "\t|-Identification : %d\n",ntohs(ip->id));
    fprintf(output , "\t|-Time To Live : %d\n",(unsigned int)ip->ttl);
    fprintf(output , "\t|-Protocol : %d\n",(unsigned int)ip->protocol);
    fprintf(output , "\t|-Header Checksum : %d\n",ntohs(ip->check));
    fprintf(output , "\t|-Source IP : %s\n", inet_ntoa(source.sin_addr));
    fprintf(output , "\t|-Destination IP : %s\n",inet_ntoa(dest.sin_addr));
}

void sniffer_extract_transport_header(uint8_t * buffer, FILE * output)
{

    struct iphdr *ip = (struct iphdr *)( buffer + sizeof(struct ethhdr) );
    uint16_t iphdrlen = ip->ihl*4;

    fprintf(output , "\n******************** Transport Header ********************\n");

    /* getting pointer to udp header*/
    if(ip->protocol == PROTOCOL_UDP_NUM)
    {
        sniffer_extract_tcp_header(buffer,iphdrlen,output);
    }
    else if(ip->protocol == PROTOCOL_TCP_NUM)
    {
        sniffer_extract_tcp_header(buffer,iphdrlen,output);
    }
    else if(ip->protocol == PROTOCOL_ICMP_NUM)
    {
        sniffer_extract_icmp_header(buffer,iphdrlen,output);
    }

}
void sniffer_extract_data(uint8_t * buffer ,ssize_t buflen, uint16_t iphdrlen, FILE * output)
{
    uint8_t * data = (buffer + iphdrlen + sizeof(struct ethhdr) + sizeof(struct udphdr));
    int remaining_data = buflen - (iphdrlen + sizeof(struct ethhdr) + sizeof(struct udphdr));
    int i;
    fprintf(output, "\n********************        Data       ********************\n");

    for(i=0; i<remaining_data;i++)
    {
        if(i != 0 && i%16 == 0)
        {
            fprintf(output,"\n");
        }
        fprintf(output," %.2X ",data[i]);
    }
    fprintf(output,"\n********************************************************************\n");


}
void sniffer_extract_tcp_header(uint8_t *buffer, uint16_t iphdrlen, FILE *output) 
{
    struct tcphdr *tcp = (struct tcphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));
    fprintf(output , "TCP Header\n");

    fprintf(output , "\t|-Source Port          : %d\n" , ntohs(tcp->source));
    fprintf(output , "\t|-Destination Port     : %d\n" , ntohs(tcp->dest));
    fprintf(output , "\t|-Sequence Number      : %u\n" , ntohl(tcp->seq));
    fprintf(output , "\t|-Acknowledge Number   : %u\n" , ntohl(tcp->ack_seq));
    fprintf(output , "\t|-Header Length        : %d DWORDS or %d BYTES\n" , (unsigned int)tcp->doff, (unsigned int)tcp->doff * 4);

    fprintf(output , "\t|--------Flags--------\n");
    fprintf(output , "\t\t|-Urgent Flag          : %d\n" , (unsigned int)tcp->urg);
    fprintf(output , "\t\t|-Acknowledgement Flag : %d\n" , (unsigned int)tcp->ack);
    fprintf(output , "\t\t|-Push Flag            : %d\n" , (unsigned int)tcp->psh);
    fprintf(output , "\t\t|-Reset Flag           : %d\n" , (unsigned int)tcp->rst);
    fprintf(output , "\t\t|-Synchronise Flag     : %d\n" , (unsigned int)tcp->syn);
    fprintf(output , "\t\t|-Finish Flag          : %d\n" , (unsigned int)tcp->fin);

    fprintf(output , "\t|-Window size          : %d\n" , ntohs(tcp->window));
    fprintf(output , "\t|-Checksum             : %d\n" , ntohs(tcp->check));
    fprintf(output , "\t|-Urgent Pointer       : %d\n" , tcp->urg_ptr);
}

void sniffer_extract_udp_header(uint8_t *buffer, uint16_t iphdrlen,FILE *output) 
{
    struct udphdr *udp = (struct udphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));

    fprintf(output , "\t|-Source Port : %d\n" , ntohs(udp->source));
    fprintf(output , "\t|-Destination Port : %d\n" , ntohs(udp->dest));
    fprintf(output , "\t|-UDP Length : %d\n" , ntohs(udp->len));
    fprintf(output , "\t|-UDP Checksum : %d\n" , ntohs(udp->check));

}

void sniffer_extract_icmp_header(uint8_t *buffer, uint16_t iphdrlen , FILE *output) 
{
    struct icmphdr *icmp = (struct icmphdr*)(buffer + iphdrlen + sizeof(struct ethhdr));

    fprintf(output , "ICMP Header\n");
    fprintf(output , "\t|-Type     : %d\n" , (uint32_t)(icmp->type));
    fprintf(output , "\t|-Code     : %d\n" , (uint32_t)(icmp->code));
    fprintf(output , "\t|-Checksum : %d\n" , ntohs(icmp->checksum));
    
    if (icmp->type == ICMP_ECHO || icmp->type == ICMP_ECHOREPLY) 
    {
        fprintf(output , "\t|-Identifier       : %d\n" , ntohs(icmp->un.echo.id));
        fprintf(output , "\t|-Sequence Number  : %d\n" , ntohs(icmp->un.echo.sequence));
    }
}
uint64_t sniffer_save_raw_packet(uint8_t *buffer,ssize_t buflen, FILE * tmp_file, FILE * offset_file)
{

    packet_info new_packet_info;

    /*Get info about packet*/
    new_packet_info.id = packet_count;
    new_packet_info.packet_size = buflen;
    new_packet_info.file_offset = ftell(tmp_file);

    /*Write raw packet and packet into*/
    fflush(offset_file);
    fwrite(&new_packet_info,sizeof(packet_info),1,offset_file);
    fflush(tmp_file);
    fwrite(buffer, buflen, 1, tmp_file);

    return packet_count;
}
status sniffer_read_packet(uint64_t found_packet_id,FILE * tmp_file, FILE * offset_file, FILE * output )
{   
    status return_status = SUCCESS;
    uint8_t buffer[MAX_DATA_SIZE] = {0};
    packet_info found_packet_info;
    u_int16_t iphdrlen = 0;
    if(found_packet_id >= packet_count) /*Check if bigger then the last added id*/
    {
        return_status = FAILURE;
    }
    else
    {
        /*Get the packet info from offset file*/
        fseek(offset_file,found_packet_id*sizeof(packet_info),SEEK_SET);
        fread(&found_packet_info,sizeof(packet_info),1,offset_file);

        /*Get the packet from tmp file using packet_info*/
        fseek(tmp_file,found_packet_info.file_offset,SEEK_SET);
        fread(buffer,found_packet_info.packet_size,1,tmp_file);

        /*Format packet*/
        sniffer_exract_ethernet_header(buffer,output);
        sniffer_extract_ip_header(buffer,&iphdrlen,output);
        sniffer_extract_transport_header(buffer,output);
        sniffer_extract_data(buffer,found_packet_info.packet_size,iphdrlen,output);

        /*Go back to start of file*/
        rewind(offset_file);
        rewind(tmp_file);

    }
    return return_status;

}

void sniffer_extract_summery(uint8_t *buffer, uint64_t packet_id)
{

    struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in source ={0}, dest = {0};
    uint16_t iphdrlen = ip->ihl * 4;

    char src_ip[INET_ADDRSTRLEN];
    char dest_ip[INET_ADDRSTRLEN];

    char *protocol = NULL;
    uint16_t src_port = 0, dest_port = 0;

    source.sin_family = AF_INET;
    source.sin_addr.s_addr = ip->saddr;

    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = ip->daddr;

    inet_ntop(AF_INET, &(source.sin_addr), src_ip, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(dest.sin_addr), dest_ip, INET_ADDRSTRLEN);


    switch (ip->protocol)
    {
        case PROTOCOL_TCP_NUM:
        {
            struct tcphdr *tcp = (struct tcphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);
            src_port = ntohs(tcp->source);
            dest_port = ntohs(tcp->dest);
            protocol = "TCP";
            break;
        }
        case PROTOCOL_UDP_NUM:
        {
            struct udphdr *udp = (struct udphdr *)(buffer + sizeof(struct ethhdr) + iphdrlen);
            src_port = ntohs(udp->source);
            dest_port = ntohs(udp->dest);
            protocol = "UDP";
            break;
        }
        case PROTOCOL_ICMP_NUM:
        {
            protocol = "ICMP";
            break;
        }
        default:
            protocol = "OTHER";
            break;
    }

    printf("[%lu]\t, (%s)\t Packet from (s%s : s%u) to (d%s : d%u)\n",
           packet_id,
           protocol,
           src_ip, src_port,
           dest_ip, dest_port);
}

