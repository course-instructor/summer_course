#pragma once

#include "common.h"

int send_signup_message(int sockfd, const char *name, const char *password);

int send_login_message(int sockfd, const char *name, const char *password);

int send_room_lst_message(int sockfd);

int send_enter_room_message(int sockfd, const char* name, int room);

int send_leave_room_message(int sockfd, const char * name);

int send_in_room_message(int sockfd,const char * name, const char * str);