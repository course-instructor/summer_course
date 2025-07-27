#pragma once

#include "common.h"
#include <pthread.h>



extern enum e_highrarchy g_level;

void get_name_and_pass(char *name, char *password);


int check_action_permission(enum request_e request);


void get_input(int sockfd);

void *server_listener(void *arg);


