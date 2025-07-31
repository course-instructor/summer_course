#ifndef MY_SNIFFER
#define MY_SNIFFER
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>  
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <poll.h>
#define MAX_DATA_SIZE 65536
#define PROTOCOL_TCP_NUM 6
#define PROTOCOL_UDP_NUM 17
#define PROTOCOL_ICMP_NUM 1
#define TEMPORARY_FILE_PATH "./temporary_files/tmp_file.bin"
#define OFFSET_FILE_PATH "./temporary_files/offset_file.bin"




typedef enum status_e
{
    SUCCESS = 0,
    FAILURE  = -1,
    DISCONNECTED = -2
}status;

typedef struct packet_info_s
{
    uint64_t id;
    size_t packet_size;
    uint32_t file_offset;

}packet_info;

/**
 * @brief Start up the sniffing proccess, which reads packets and saves them in temporary file
 * @param ptr_is_sniffing pointer to boolean if sniffing is still should be active, on False its finishing listen to the last packet and exits
 * @return SUCCSESS if statup finished succesfuly otherwise return FAILED
 */
status sniffer_start();

/**
 * @brief Creates new raw socket
 * @param sock_fd File discriptor file of the new socket
 * @return Return SUCCESS if sockets a created correctly otherwise return FAILURE
 */
status sniffer_create_socket            (int * sock_fd);

/**
 * @brief Listen for incoming packets
 * @param sock_fd Socket's file discriptor
 * @param buffer The packet in raw form
 * @param buflen Length of data recived
 * @return Return SUCCESS if packet retrived succesfuly otherwise return FAILURE
 */
status sniffer_packet_reception         (int sock_fd, uint8_t *buffer, ssize_t * buflen);

/**
 * @brief Gets and prints incoming packets
 * @param sock_fd Socket's file discriptor
 * @param output Prints packet info into this
 * @return Return SUCCESS if packet retrived succesfuly otherwise return FAILURE
 */
status sniffer_packet_reader(int * sock_fd, FILE * tmp_file , FILE * offset_file);
/**
 * @brief Prints info about the frame from data link layer
 * @param buffer The packet in raw form
 * @param output Prints packet info into this
 */
void sniffer_exract_ethernet_header     (uint8_t * buffer,FILE * output);
/**
 * @brief Prints info about the packet from network layer, protocols TCP/UDP/
 * @param buffer The packet in raw form
 * @param output Prints packet info into this
 */
void sniffer_extract_ip_header          (uint8_t * buffer, u_int16_t * iphdrlen, FILE * output);
/**
 * @brief Prints info about the segment from network layer
 * @param buffer The packet in raw form
 * @param output Prints packet info into this
 */
void sniffer_extract_transport_header   (uint8_t * buffer, FILE * output);
/**
 * @brief Prints data in hexadecimal from packet
 * @param buffer The packet in raw form
 * @param output Prints packet info into this
 */
void sniffer_extract_data               (uint8_t * buffer ,ssize_t buflen, uint16_t iphdrlen, FILE * output);
/**
 * @brief Prints info about tcp header
 * @param buffer The packet in raw form
 * @param iphdrlen length of the packet header
 * @param output Prints packet info into this
 */
void sniffer_extract_tcp_header         (uint8_t *buffer, uint16_t iphdrlen,FILE * output);
/**
 * @brief Prints info about udp header
 * @param buffer The packet in raw form
 * @param iphdrlen length of the packet header
 * @param output Prints packet info into this
 */
void sniffer_extract_udp_header         (uint8_t *buffer, uint16_t iphdrlen,FILE * output);
/**
 * @brief Prints info about icmp header
 * @param buffer The packet in raw form
 * @param iphdrlen length of the packet header
 * @param output Prints packet info into this
 */
void sniffer_extract_icmp_header        (uint8_t *buffer, uint16_t iphdrlen,FILE * output);
/**
 * @brief Prints to user id, type of protocol source ip and icmp
 * @param buffer The packet in raw form
 * @param iphdrlen length of the packet header
 * @param output Prints packet info into this
 */
void sniffer_extract_summery(uint8_t *buffer, uint64_t packet_id);

/**
 * @brief Saves unformated packet into a file as well as it's ID, size and offset into.
 * @param buffer The packet in raw form
 * @param buflen Length of packet
 * @param tmp_file Saves raw packets into this file
 * @param offset_file Saves ID, size and offset into this file , for navigation inside tmp_file
 * @return Saved packet's ID
 */
uint64_t sniffer_save_raw_packet(uint8_t *buffer,ssize_t buflen, FILE * tmp_file, FILE * offset_file);
/**
 * @brief Read packet from temporary file
 * @param packet_id ID of packet to read
 * @param buflen Length of buffer
 * @param tmp_file File with raw packets 
 * @param offset_file File with ID, size and offset into this file , for navigation inside tmp_file
 * @return returns SUCCESS if got packet successfuly , otherwise returns FAILURE
 */
status sniffer_read_packet(uint64_t packet_id,FILE * tmp_file, FILE * offset_file, FILE * output );

/**
 * @brief For getting the last add packet id
 * @return lasta added packet id
 */
uint64_t * sniffer_get_current_id(void);

/**
 * @brief Listen for incoming key input from user and activated the corresponding command
 */
void input_listener(void);

/**
 * @brief Starts a thread of sniffing proccess
 * @param sniffer_thread thread to create
 */
void input_start(pthread_t * sniffer_thread);
/**
 * @brief Kills a thread of sniffing proccess
 * @param sniffer_thread thread to kill
 */
void input_kill(pthread_t * sniffer_thread);

/**
 * @brief Foramt raw packet into by user's specified id.
 */
void input_inspect(void);

/**
 * @brief Erase the temporary saved packets
 */
void input_erase(void);

/**
 * @brief Saves and formats packets from temporary file
 */
void input_dump(void);
/**
 * @brief Sets teminal into the defult mode
 */
void input_terminal_raw_mode_disable(void);
/**
 * @brief Sets teminal into getting input instantly as its entered
 */
void input_terminal_raw_mode_enable(void);
/**
 * @brief Prints help commands
 */
void input_help(void);

status input_get_id(uint64_t * ptr_input_id);


#endif