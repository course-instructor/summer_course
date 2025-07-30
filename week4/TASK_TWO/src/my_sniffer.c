#include "my_sniffer.h"


_Atomic bool_e g_listen_to_socket = FALSE;
_Atomic int g_message_id ;
_Atomic bool_e g_end = FALSE;


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
        FILE * log_index_f = fopen(LOG_MESSAGE_INDEX_FILE, "w");
        if(log_file == NULL || log_index_f == NULL)
        {
            perror("my_sniffer_create_socket opening log files\n");
            ret = FALSE;
        }
        else
        {
            while(g_listen_to_socket )
            {
                my_sniffer_listn_socket(sock_r, log_file, log_index_f);
            }

            fclose(log_file);
            fclose(log_index_f);
        }

    }
    return ret;
}

bool_e  my_sniffer_listn_socket(int sock_r, FILE * log_f, FILE * log_index_f)
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
        /* write the starting index of the file to the index log file to find the message later */
        fseek(log_f, 0, SEEK_END);
        int start_index = ftell(log_f);
        fprintf(log_index_f, "%d\n", start_index);

        my_sniffer_print_ethernet(buffer, log_f);
    }
    return ret;
}

int my_sniffer_get_message_offset(FILE * log_index_f, int message_id)
{
    int counter = 0;
    int offset = ERROR;
    while(!feof(log_index_f) && (offset == ERROR))
    {
        if(counter == message_id)
        {
            fscanf(log_index_f, "%d", &offset);
        }
        counter++;
    }
    return offset;
}

void my_sniffer_print_packet(int message_id)
{
    FILE * log_f = fopen(LOG_FILE, "r");
    FILE * log_index_f = fopen(LOG_MESSAGE_INDEX_FILE, "r");
    if(log_f == NULL || log_index_f == NULL)
    {
        perror("my_sniffer_print_packet opening log files\n");
    }
    else
    {
        int offset = my_sniffer_get_message_offset(log_index_f, message_id);
        if(offset != ERROR)
        {
            fseek(log_f, offset, SEEK_SET);
            char buffer[PACKET_SIZE] = {'\0'};

            do
            {
                if (fgets(buffer, PACKET_SIZE, log_f) == NULL)
                {
                    break;
                }
                printf("%s", buffer);
            } while (!strstr(buffer,MESSAGE_END_LINE));
        }
        else
        {
            perror("my_sniffer_print_packet: message_id not found\n");
        }

        fclose(log_f);
        fclose(log_index_f);
    }
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
    my_sniffer_print_header_line(buffer, log_f);

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

void my_sniffer_print_header_line(unsigned char * buffer, FILE * log_f)
{
    /* skipping the ethernet header to get the type of message */
    buffer += sizeof(struct ethhdr);
    struct iphdr *ip_header = (struct iphdr *)(buffer );
    const char * header_str;
    switch (ip_header->protocol)
    {
    case IP_TCP:
        header_str = MESSAGE_TYPE_TO_STR[TCP];
        break;
    case IP_UDP:
        header_str = MESSAGE_TYPE_TO_STR[UDP];
        break;
    case IP_ICMP:
        header_str = MESSAGE_TYPE_TO_STR[ICMP];
        break;
    default:
        header_str = "UNKNOWN";
        break;
    }

    fprintf(log_f, "***********************%s***********************\n", header_str);
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
            my_sniffer_print_icmp(buffer, message, log_f);
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

void my_sniffer_print_icmp(unsigned char * buffer, message_s message, FILE * log_f)
{
    struct icmphdr *icmp_header = (struct icmphdr *)(buffer);

    fprintf(log_f, "ICMP Header\n");
    fprintf(log_f, "\t|-Type                  : %d\n", (unsigned int)icmp_header->type);
    fprintf(log_f, "\t|-Code                  : %d\n", (unsigned int)icmp_header->code);
    fprintf(log_f, "\t|-Checksum              : %d\n", ntohs(icmp_header->checksum));

    switch (icmp_header->type)
    {
        case ICMP_ECHOREPLY:
        case ICMP_ECHO:
            fprintf(log_f, "\t\t|-ID                    : %d\n", ntohs(icmp_header->un.echo.id));
            fprintf(log_f, "\t\t|-Sequence              : %d\n", ntohs(icmp_header->un.echo.sequence));
            break;

        case ICMP_DEST_UNREACH:
            if(icmp_header->code == ICMP_FRAG_NEEDED)
            {
                fprintf(log_f, "\t\t|-MTU                  : %d\n", ntohs(icmp_header->un.frag.mtu));
            }
            break;

        case ICMP_REDIRECT:
            fprintf(log_f, "\t\t|-Gateway Address       : %s\n", inet_ntoa(*(struct in_addr *)&icmp_header->un.gateway));
            break;

    }

    fprintf(log_f, "\n");

    my_sniffer_print_message(message);

    message.data_length -= sizeof(struct icmphdr);
    buffer += sizeof(struct icmphdr);

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

void* my_sniffer_listen_for_input_thread(void *)
{

    struct termios oldt, newt;

    /* Save terminal settings */
    tcgetattr(STDIN_FILENO, &oldt);

    /* remove waiting for enter */
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    key_input_e state = ESCAPE;
    do
    {
        my_sniffer_handle_input(&state);
    } while (!g_end);


    /* Restore terminal settings */
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return NULL;
}

void my_sniffer_handle_input(key_input_e * state_ptr)
{
    key_input_e key_input;
    int id = ERROR;

    switch (*state_ptr)
    {
        case START:
            printf("enter %c to stop siffing\n", STOP);
            key_input = getchar();
            if(key_input == STOP)
            {
                * state_ptr = STOP;
                g_listen_to_socket = FALSE;
            }
            break;
        case STOP:
            printf("enter %c to start sniffing, or %c to inspect\n", START, INSPECT);
            key_input = getchar();
            if(key_input == START)
            {
                * state_ptr = START;
                g_listen_to_socket = TRUE;
            }
            else if(key_input == INSPECT)
            {
                * state_ptr = INSPECT;
            }
            break;
        case INSPECT:
            printf("enter the number of message u want to inspect, or %c to stop inspecting\n", ESCAPE);
            while(1)
            {
                key_input = getchar();
                if (key_input >= '0' && key_input <= '9')
                {
                    if(id == ERROR)
                    {
                        id = 0;
                    }
                    id *= 10;
                    id += key_input - '0';
                    printf("%d\n", id);
                }
                else if (key_input == ESCAPE)
                {
                    * state_ptr = ESCAPE;
                    break;
                }
                else if(id != ERROR)
                {
                    my_sniffer_print_packet(id);
                    id = ERROR;
                }
            }
            break;
        case ESCAPE:
            printf("enter %c to start sniffing\n", START);
            key_input = getchar();
            if(key_input == START)
            {
                * state_ptr = START;
                g_listen_to_socket = TRUE;
            }
            break;
        default:
            perror("unknown state\n");
            break;
    }
}


void* my_sniffer_sniffer_thread(void *)
{
    bool_e exit = FALSE;

    while(g_end == FALSE && exit == FALSE)
    {
        if(g_listen_to_socket == TRUE && exit == FALSE)
        {
            exit = my_sniffer_create_socket();
        }

    }
    return NULL;
}
int main()
{
    pthread_t input_thread;
    pthread_t sniffer_thread;

    pthread_create(&input_thread, NULL, my_sniffer_listen_for_input_thread, NULL);
    pthread_create(&sniffer_thread, NULL, my_sniffer_sniffer_thread, NULL);

    pthread_join(input_thread, NULL);
    pthread_join(sniffer_thread, NULL);

    return 0;
}
