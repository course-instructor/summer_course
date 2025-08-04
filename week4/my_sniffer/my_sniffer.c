#define _GNU_SOURCE
#include <stdio.h>
#include "my_sniffer.h"

/*global varibles*/
uint64_t packet_count = 0;
bool is_sniffing = false;
bool is_inspect_mode = false;
bool is_running = false;

pthread_t sniffer_thread;
pthread_t reception_thread;

int main()
{
    if(atexit((void*)catch_function)!= SUCCESS)
    {
        fprintf(stderr, "Failed to register the tutorialspoint function with atexit\n");
    }
    
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
void catch_function(int signo) 
{
    if(signo == SIGINT)
    {
        if(is_sniffing == true)
        {
            is_sniffing = false;
            is_inspect_mode = false;
            is_running = false;
            pthread_cancel(reception_thread);
            pthread_join(sniffer_thread,NULL);
            
        }
        
        exit(0);

    }
}
