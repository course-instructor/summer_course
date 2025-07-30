#ifndef my_sniffer_h
#define my_sniffer_h
#include <stdio.h>
#include <stddef.h>
#include <time.h>

#define DATA_SIZE_g 65536
#define PROTCOL_LEN_G 5
#define IP_LEN_G 20
#define MAX_PACKETS_G 65500
#define NULL ((void *)0)
#define STDIN_FILENO 0
#define TCP_NUM 6
#define UDP_NUM 17
#define ICMP_NUM 1
enum status {FAIL, SUCCESS};
enum state {OFF,ON};

extern char buffer_g[DATA_SIZE_g];  
extern int packet_id_g;
extern char* filepath_packets_data;
extern char* filepath_info_packet;


typedef struct
{
    int id;
    char protocol[PROTCOL_LEN_G];
    char source_ip[IP_LEN_G];
    char destination_ip[IP_LEN_G];
    int src_port;
    int dest_port;
    long starting_location_in_file;
    long end_location_in_file;
    long num_lines;
}data_packet;

/**
 * @brief this function opens a raw socket to capture packets.
 * 
 * @return int 
 */

int my_sniffer_open_socket();

/**
 * @brief this function receives packets from the opened socket.
 * 
 * @param sock_r the socket file descriptor.
 * @return int 
 */

int my_sniffer_recieve(int sock_r);

/**
 * @brief this function prints the Ethernet header of the captured packet to the specified file.
 * 
 * @param file_packet_log the file pointer to write the Ethernet header.
 * @return int 
 */

int my_sniffer_print_ethheader(FILE * file_packet_log);

/**
 * @brief this function prints the IP header of the captured packet to the specified file.
 * 
 * @param file_packet_log this is the file pointer to write the IP header.
 * @param packet_info_to_print we fill here the source and destination IPs.
 * @return int 
 */

int my_sniffer_print_ipheader(FILE * file_packet_log, data_packet* packet_info_to_print);

/**
 * @brief this function determines if the packet is TCP or UDP and writes the protocol type to the file.
 * 
 * @param file_packet_log this is the file pointer to write the protocol type.
 * @param packet_info_to_print we fill here the protocol type.
 * @return int 
 */

int my_sniffer_print_transport_header(FILE * file_packet_log, data_packet* packet_info_to_print);

/**
 * @brief this function prints the data of the packet to the specified file.
 * 
 * @param file_packet_log this is the file pointer to write the data.
 * @param packet_info_to_print this is the data packet structure that contains information about the packet.
 * @return int 
 */

int my_sniffer_print_data(FILE * file_packet_log, data_packet* packet_info_to_print);

/**
 * @brief this function determines if the packet is TCP or UDP and writes the protocol type to the file.
 * 
 * @param file_packet_log this is the file pointer to write the protocol type.
 * @param packet_info_to_print this is the data packet structure that contains information about the packet.
 * @return int 
 */

int my_sniffer_tcp_or_udp(FILE * file_packet_log, data_packet *packet_info_to_print );

/**
 * @brief this function runs the packet sniffer, capturing packets and writing their information to files.
 * 
 * @return int 
 */

int packet_runner();

/**
 * @brief this function copies the requested packet information based on the line ID from the info file and prints it.
 * 
 * @param line_id the ID of the packet to copy.
 * @return int 
 */

int copy_requested_packet(int line_id);

/**
 * @brief this function reads input from stdin and handles commands to start, stop, or inspect packets.
 * 
 * @param arg the pressed key from stdin.
 * @return void* 
 */

void *read_stdin(void *arg); 

#endif // my_sniffer_h
