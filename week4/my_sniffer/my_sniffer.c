#define _GNU_SOURCE
#include <stdio.h>
#include "include/my_sniffer.h"

/*global varibles*/
uint64_t packet_count = 0;
bool is_sniffing = false;

int main()
{
 
    input_listener();
    return 0;
    
}
