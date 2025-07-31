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


/* the struct contains all the data that is shown to the user in listen mode*/
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

/**
 * @brief the function creates the log files, starts listening to the socket and calls a de-encapsulation function
 *
 * @return bool_e return FALSE if there was some eror for example opening files...
 */
bool_e my_sniffer_create_and_listen_socket(void);

/**
 * @brief
 *
 * @param sock_r the socket
 * @param log_f the file to write the formatted packets to
 * @param log_index_f the file to write the starting indexes of each packet in the log file
 * @return bool_e FALSE if there were any errors
 */
bool_e  my_sniffer_get_packet_socket(int sock_r, FILE * log_f, FILE * log_index_f);

/**
 * @brief the function returns the offset of the requested message from the start of the log file
 *
 * @param log_index_f the file with the starting indexes of each packet in the log file
 * @param message_id the number of message we want to get the offset to the start of
 * @return int
 */
int my_sniffer_get_message_offset(FILE * log_index_f, int message_id);

/**
 * @brief the function prints the requested message from the log file from start (found with message_ofset func) up to the ending line
 *
 * @param message_id the number of the message to be printed
 */
void my_sniffer_print_packet(int message_id);

/**
 * @brief the function gets th unformatted mac address field from the ethernet header and writes it to the log file
 *
 * @param address_arr the mac address
 * @param log_f the log file
 */
void my_sniffer_print_ethernet_addr(unsigned char address_arr [ETH_ALEN], FILE * log_f);

/**
 * @brief the function formats, writes the ethernet header into the log file, moves buffer and calls the next de-encapsulation function
 *
 * @param buffer the message from the raw socket
 * @param log_f the log file
 */
void my_sniffer_print_ethernet(unsigned char * buffer, FILE * log_f);

/**
 * @brief the function writes the header line by getting the type of the protocol (tcp,udp,icmp) from the ip header into the log file
 *
 * @param buffer the message from the raw socket (already pointing to the ip header)
 * @param log_f the log file
 */
void my_sniffer_print_header_line(unsigned char * buffer, FILE * log_f);

/**
 * @brief the function formats, writes the ip header into the log file, moves buffer and calls the next de-encapsulation function
 *
 * @param buffer the message from the raw socket (already pointing to the ip header)
 * @param message a struct that contains info from the previos layers and is passed to the next function (the info to be printed to the user in listen mode)
 * @param log_f the log file
 */
void my_sniffer_print_ip(unsigned char * buffer, message_s message, FILE * log_f);

/**
 * @brief the function formats, writes the tcp header into the log file, moves buffer and calls the data handling function
 *
 * @param buffer the message from the raw socket (already pointing to the tcp header)
 * @param message a struct that contains info from the previos layers and is passed to the print message function (the info to be printed to the user in listen mode)
 * @param log_f the log file
 */
void my_sniffer_print_tcp(unsigned char * buffer, message_s message, FILE * log_f);

/**
 * @brief the function formats, writes the udp header into the log file, moves buffer and calls the data handling function
 *
 * @param buffer the message from the raw socket (already pointing to the udp header)
 * @param message a struct that contains info from the previos layers and is passed to the print message function (the info to be printed to the user in listen mode)
 * @param log_f the log file
 */
void my_sniffer_print_udp(unsigned char * buffer, message_s message, FILE * log_f);


/**
 * @brief the function formats, writes the icmp header into the log file, moves buffer and calls the data handling function
 *
 * @param buffer the message from the raw socket (already pointing to the icmp header)
 * @param message a struct that contains info from the previos layers and is passed to the print message function (the info to be printed to the user in listen mode)
 * @param log_f the log file
 */
void my_sniffer_print_icmp(unsigned char * buffer, message_s message, FILE * log_f);

/**
 * @brief the function prints info about the package that was written to thelog file
 *
 * @param message a struct containing all the data that needs to be printed
 */
void my_sniffer_print_message(message_s message);

/**
 * @brief the function prints the data at the end of the package
 *
 * @param buffer a buffer pointing to the start of the data
 * @param data_length how much data should be read
 * @param log_f the log file to write to
 */
void my_sniffer_print_data(unsigned char * buffer, int data_length, FILE * log_f);

/**
 * @brief a thread that listens to key presses and kepps track of the state the user is in (listening, inspecting ect...)
 *
 * @return void* NULL
 */
void* my_sniffer_listen_for_input_thread(void *);

/**
 * @brief the function gets a single key press and handles it, switching states if needed
 *
 * @param state_ptr
 */
void my_sniffer_handle_input(key_input_e * state_ptr);

/**
 * @brief a thread that runs the listen to socket function when in the listen state
 *
 * @return void* NULL
 */
void* my_sniffer_sniffer_thread(void *);
#endif // MY_SNIFFER_H
