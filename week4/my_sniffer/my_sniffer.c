#define _GNU_SOURCE
#include <stdio.h>
#include "include/my_sniffer.h"

int main()
{
    int sock_fd;
    
    status return_status = SUCCESS;

    if(sniffer_create_socket(&sock_fd) == FAILURE)
    {
        return_status = FAILURE;
    }
    else
    {
        while (return_status == SUCCESS)
        {
            return_status = sniffer_packet_reader(&sock_fd,stdout);
        }
        

    }
    
    
    return return_status;
    
}
