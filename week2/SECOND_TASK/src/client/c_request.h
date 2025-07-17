#pragma once

#include "common.h"

int send_signup_message(int sockfd, const char *name, const char *password);

int send_login_message(int sockfd, const char *name, const char *password);

int send_room_lst_message(int sockfd);

int send_enter_room(int sockfd, int room_num, const char *name);
