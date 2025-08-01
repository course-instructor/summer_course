#define _GNU_SOURCE
#include <stdio.h>
#include "my_sniffer.h"

/*global varibles*/
uint64_t packet_count = 0;
bool is_sniffing = false;

int main()
{
    if (geteuid() == 0) 
    {
        input_listener();
    } 
    else 
    {
        fprintf(stderr,"ERROR : Not running as root.\n");
    }
    return 0;
    
}
