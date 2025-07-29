#include "my_sniffer.h"


bool_e g_listen_to_socket = TRUE;
int g_message_id = -1;

bool_e my_sniffer_create_socket(void)
{
    bool_e ret = TRUE;
    g_message_id = 1;
    int sock_r;

    sock_r = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    if(sock_r == -1)
    {
        perror("my_sniffer_create_socket creating socket\n");
        ret = FALSE;
    }
    else
    {
        FILE * log_file = fopen(LOG_FILE,"w");
        FILE * log_index_file = fopen(LOG_MESSAGE_INDEX_FILE, "w");
        if(log_file == NULL || log_index_file == NULL)
        {
            perror("my_sniffer_create_socket opening log files\n");
            ret = FALSE;
        }
        else
        {
            while(g_listen_to_socket)
            {
                my_sniffer_listn_socket(sock_r, log_file);
            }

            fclose(log_file);
            fclose(log_index_file);
        }

    }
    return ret;
}

bool_e  my_sniffer_listn_socket(int sock_r, FILE * log_f)
{
    bool_e ret = TRUE;
    int buffer_length;
    unsigned char buffer[PACKET_SIZE] = {0};

    struct sockaddr saddr;
    int saddr_len = sizeof (saddr);

    buffer_length = recvfrom(sock_r, buffer,PACKET_SIZE,0, & saddr,(socklen_t *) (& saddr_len));

    if(buffer_length == -1)
    {
        perror("my_sniffer_listn_socket buffer_length < 0\n");
        ret = FALSE;
    }
    else
    {
        my_sniffer_print_ethernet(buffer, log_f);
    }
    return ret;
}

void my_sniffer_print_ethernet_addr(unsigned char address_arr [ETH_ALEN], FILE * log_f)
{
    for(int i = 0; i < ETH_ALEN - 1; i++)
    {
        fprintf(log_f, "%02x-", (unsigned int)address_arr[i]);
    }
    fprintf(log_f, "%02x\n", (unsigned int)address_arr[ETH_ALEN - 1]);
}

void my_sniffer_print_ethernet(unsigned char * buffer, FILE * log_f)
{
    struct ethhdr *ethernet_header = (struct ethhdr *)(buffer);

    fprintf(log_f, "Ethernet Header\n");
    fprintf(log_f, "\t|-Source Address      : ");
    my_sniffer_print_ethernet_addr(ethernet_header->h_source, log_f);
    fprintf(log_f, "\t|-Destination Address : ");
    my_sniffer_print_ethernet_addr(ethernet_header->h_dest, log_f);
    fprintf(log_f, "\t|-Protocol            : %d\n", ethernet_header->h_proto);
    fprintf(log_f, "\n");

    message_s message;
    message.message_id = g_message_id++;

    buffer += sizeof(struct ethhdr);

    my_sniffer_print_ip(buffer, message, log_f);
}

void my_sniffer_print_ip(unsigned char * buffer, message_s message, FILE * log_f)
{
    struct iphdr *ip_header = (struct iphdr *)(buffer );




    fprintf(log_f, "IP Header\n");
    fprintf(log_f, "\t|-Version           : %d\n", (unsigned int)ip_header->version);
    fprintf(log_f, "\t|-Internet Header Length : %d DWORDS or %d Bytes\n", (unsigned int)ip_header->ihl, ((unsigned int)(ip_header->ihl))*DWORD_SIZE);
    fprintf(log_f, "\t|-Type Of Service   : %d\n", (unsigned int)ip_header->tos);
    fprintf(log_f, "\t|-Total Length      : %d Bytes\n", ntohs(ip_header->tot_len));
    fprintf(log_f, "\t|-Identification    : %d\n", ntohs(ip_header->id));
    fprintf(log_f, "\t|-Time To Live      : %d\n", (unsigned int)ip_header->ttl);
    fprintf(log_f, "\t|-Protocol          : %d\n", (unsigned int)ip_header->protocol);
    fprintf(log_f, "\t|-Header Checksum   : %d\n", ntohs(ip_header->check));
    fprintf(log_f, "\t|-Source IP         : %s\n", inet_ntoa(*(struct in_addr *)&ip_header->saddr));
    fprintf(log_f, "\t|-Destination IP    : %s\n", inet_ntoa(*(struct in_addr *)&ip_header->daddr));
    fprintf(log_f, "\n");

    buffer += (unsigned int)ip_header->ihl * DWORD_SIZE;

    message.data_length = ntohs(ip_header->tot_len) - (unsigned int)ip_header->ihl * DWORD_SIZE;

    message.message_src_ip = strdup(inet_ntoa(*(struct in_addr *)&ip_header->saddr));
    message.message_dest_ip = strdup(inet_ntoa(*(struct in_addr *)&ip_header->daddr));

    switch (ip_header->protocol)
    {
        case IP_TCP:
            message.message_type = TCP;
            my_sniffer_print_tcp(buffer, message, log_f);
            break;
        case IP_UDP:
            message.message_type = UDP;
            my_sniffer_print_udp(buffer, message, log_f);
            break;
        case IP_ICMP:
            message.message_type = ICMP;
            break;

        default:
            perror("my_sniffer_print_ip: Unknown protocol\n");
            fprintf(log_f, "%s\n", MESSAGE_END_LINE);
            fflush(log_f);
            break;
    }

}

void my_sniffer_print_tcp(unsigned char * buffer, message_s message, FILE * log_f)
{
    struct tcphdr *tcp_header = (struct tcphdr *)(buffer);

    message.message_src_port = ntohs(tcp_header->source);
    message.message_dest_port = ntohs(tcp_header->dest);

    fprintf(log_f, "TCP Header\n");
    fprintf(log_f, "\t|-Source Port             : %d\n", message.message_src_port);
    fprintf(log_f, "\t|-Destination Port        : %d\n", message.message_dest_port);
    fprintf(log_f, "\t|-Sequence Number         : %u\n", ntohl(tcp_header->seq));
    fprintf(log_f, "\t|-Acknowledgment Number   : %u\n", ntohl(tcp_header->ack_seq));
    fprintf(log_f, "\t|-Header Length           : %d DWORDS or %d Bytes\n", (unsigned int)tcp_header->doff, ((unsigned int)(tcp_header->doff))*DWORD_SIZE);
    fprintf(log_f, "\t|----------Flags-------------\n");
    fprintf(log_f, "\t\t|-Urgent Flag           : %d\n", (unsigned int)tcp_header->urg);
    fprintf(log_f, "\t\t|-Acknowledgment Flag   : %d\n", (unsigned int)tcp_header->ack);
    fprintf(log_f, "\t\t|-Push Flag             : %d\n", (unsigned int)tcp_header->psh);
    fprintf(log_f, "\t\t|-Reset Flag            : %d\n", (unsigned int)tcp_header->rst);
    fprintf(log_f, "\t\t|-Synchronize Flag      : %d\n", (unsigned int)tcp_header->syn);
    fprintf(log_f, "\t\t|-Finish Flag           : %d\n", (unsigned int)tcp_header->fin);
    fprintf(log_f, "\t|-Window Size             : %d\n", ntohs(tcp_header->window));
    fprintf(log_f, "\t|-Checksum                : %d\n", ntohs(tcp_header->check));
    fprintf(log_f, "\t|-Urgent Pointer          : %d\n", ntohs(tcp_header->urg_ptr));
    fprintf(log_f, "\n");

    buffer += (unsigned int)tcp_header->doff * DWORD_SIZE;

    message.data_length -= (unsigned int)tcp_header->doff * DWORD_SIZE;

    my_sniffer_print_message(message);

    my_sniffer_print_data(buffer, message.data_length, log_f);
}

void my_sniffer_print_udp(unsigned char * buffer, message_s message, FILE * log_f)
{
    struct udphdr *udp_header = (struct udphdr *)(buffer );

    message.message_src_port = ntohs(udp_header->source);
    message.message_dest_port = ntohs(udp_header->dest);

    fprintf(log_f, "UDP Header\n");
    fprintf(log_f, "\t|-Source Port             : %d\n", message.message_src_port);
    fprintf(log_f, "\t|-Destination Port        : %d\n", message.message_dest_port);
    fprintf(log_f, "\t|-UDP Length              : %d Bytes\n", ntohs(udp_header->len));
    fprintf(log_f, "\t|-UDP Checksum            : %d\n", ntohs(udp_header->check));
    fprintf(log_f, "\n");

    buffer += sizeof(struct udphdr);

    my_sniffer_print_message(message);

    message.data_length -= sizeof(struct udphdr);
    my_sniffer_print_data(buffer, message.data_length, log_f);
}

void my_sniffer_print_message(message_s message)
{
    printf("[%d](%s) packet from %s:%d to %s:%d\n",
    message.message_id, (char *)MESSAGE_TYPE_TO_STR[message.message_type], message.message_src_ip, message.message_src_port, message.message_dest_ip, message.message_dest_port);
}

void my_sniffer_print_data(unsigned char * buffer, int data_length, FILE * log_f)
{
    fprintf(log_f, "Data\n");
    for(int i = 0; i < data_length; i++)
    {
        fprintf(log_f, " %02x ", buffer[i]);
        if( (i + 1) % DATA_WORDS_IN_LINE == 0)
        {
            fprintf(log_f, "\n");
        }
    }
    fprintf(log_f, "\n%s\n", MESSAGE_END_LINE);
    fflush(log_f);
}

int main()
{
    my_sniffer_create_socket();

    return 0;
}
