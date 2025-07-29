#ifndef MU_SNIFFER
#define MY_SNIFFER
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>  
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>
#define MAX_DATA_SIZE 65536
#define PROTOCOL_TCP_NUM 6
#define PROTOCOL_UDP_NUM 17
#define PROTOCOL_ICMP_NUM 1

typedef enum status_e
{
    SUCCESS = 0,
    FAILURE  = -1,
    DISCONNECTED = -2
}status;

/**
 * @brief Creates new raw socket
 * @param sock_fd File discriptor file of the new socket
 * @return Return SUCCESS if sockets a created correctly otherwise return FAILURE
 */
status sniffer_create_socket            (int * sock_fd);
status sniffer_packet_reception         (int sock_fd, uint8_t *buffer, ssize_t * buflen);

status sniffer_packet_reader(int *sock_fd, FILE * output);

void sniffer_exract_ethernet_header     (uint8_t * buffer,FILE * output);
void sniffer_extract_ip_header          (uint8_t * buffer, u_int16_t * iphdrlen, FILE * output);
void sniffer_extract_transport_header   (uint8_t * buffer, FILE * output);
void sniffer_extract_data               (uint8_t * buffer ,ssize_t buflen, uint16_t iphdrlen, FILE * output);

void sniffer_extract_tcp_header         (uint8_t *buffer, uint16_t iphdrlen,FILE * output);
void sniffer_extract_udp_header         (uint8_t *buffer, uint16_t iphdrlen,FILE * output);
void sniffer_extract_icmp_header        (uint8_t *buffer, uint16_t iphdrlen,FILE * output);

#endif