#ifndef MY_SNIFFER_H
#define MY_SNIFFER_H
#define _GNU_SOURCE

#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdio.h>

#include <termios.h>
#include <unistd.h>
#include <pthread.h>


#define PACKET_SIZE 65536 /* 2^16 (lenght is 2 bytes) */
#define DWORD_SIZE 4 /* 1 DWORD = 4 Bytes */
#define DATA_WORDS_IN_LINE 16 /* number of words in line when printing the data */

#define LOG_FILE "my_sniffer_log.txt"
#define LOG_MESSAGE_INDEX_FILE "my_sniffer_log_index.txt"

#define MESSAGE_END_LINE  "*********************************************"

#define IP_TCP 6
#define IP_UDP 17
#define IP_ICMP 1

#define ERROR -1

const char *  MESSAGE_TYPE_TO_STR [3] = {"TCP", "UDP", "ICMP"};

typedef enum
{
    FALSE, TRUE
} bool_e;

typedef enum
{
    TCP, UDP, ICMP
} message_type_e;

typedef enum
{
    START = 's',
    STOP = 'k',
    INSPECT = 'i',
    ESCAPE = 27, /* Escape key */
}key_input_e;



typedef struct
{
    int message_id;
    message_type_e message_type;

    char * message_src_ip;
    int message_src_port;

    char * message_dest_ip;
    int message_dest_port;

    int data_length;
} message_s;


bool_e my_sniffer_create_socket(void);
bool_e  my_sniffer_listn_socket(int sock_r, FILE * log_f, FILE * log_index_f);
int my_sniffer_get_message_offset(FILE * log_index_f, int message_id);
void my_sniffer_print_packet(int message_id);
void my_sniffer_print_ethernet_addr(unsigned char address_arr [ETH_ALEN], FILE * log_f);
void my_sniffer_print_ethernet(unsigned char * buffer, FILE * log_f);
void my_sniffer_print_header_line(unsigned char * buffer, FILE * log_f);
void my_sniffer_print_ip(unsigned char * buffer, message_s message, FILE * log_f);
void my_sniffer_print_tcp(unsigned char * buffer, message_s message, FILE * log_f);
void my_sniffer_print_udp(unsigned char * buffer, message_s message, FILE * log_f);
void my_sniffer_print_icmp(unsigned char * buffer, message_s message, FILE * log_f);
void my_sniffer_print_message(message_s message);
void my_sniffer_print_data(unsigned char * buffer, int data_length, FILE * log_f);
void* my_sniffer_listen_for_input_thread(void *);
void my_sniffer_handle_input(key_input_e * state_ptr);
void* my_sniffer_sniffer_thread(void *);
#endif // MY_SNIFFER_H
