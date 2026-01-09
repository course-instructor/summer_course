#include "./HEADER/my_sniffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int main()
{
    pthread_t listening_thread;
    pthread_create(&listening_thread, NULL, read_stdin, NULL);
    pthread_join(listening_thread, NULL);
    return 0;

}