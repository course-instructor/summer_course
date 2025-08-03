#define _GNU_SOURCE
#include <stdio.h>
#include "my_sniffer.h"

/*global varibles*/
uint64_t packet_count = 0;
bool is_sniffing = false;
pthread_t sniffer_thread;

int main()
{
    signal(SIGINT , catch_function);

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
static void catch_function(int signo) 
{
    if(signo == SIGINT)
    {
        if(is_sniffing == true)
        {
            is_sniffing = false;
            pthread_join(sniffer_thread,NULL);
        }
        exit(0);

    }
}
