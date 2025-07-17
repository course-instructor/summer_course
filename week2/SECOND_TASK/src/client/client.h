#pragma once

#include "common.h"
#include <pthread.h>

#define CLIENT_PORT "3490"

enum e_highrarchy { NOT_REGISTERED, LOGGED_IN, IN_ROOM };

extern enum e_highrarchy g_level;

/**
 * Prompt user for username and password.
 */
void get_name_and_pass(char *name, char *password);

/**
 * Check if a request is allowed in the current hierarchy.
 * @return non-zero if permitted
 */
int check_action_permission(enum request_e request);

/**
 * Main UI input loop.
 */
void get_input(int sockfd);

/**
 * Thread function to listen for server messages.
 */
void *server_listener(void *arg);

