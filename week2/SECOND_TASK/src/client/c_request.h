#pragma once

#include "common.h"

void send_signup_message(int sockfd, const char *name, const char *password);

void send_login_message(int sockfd, const char *name, const char *password);

void send_room_lst_message(int sockfd);

void send_enter_room_message(int sockfd, const char* name, int room);

void send_leave_room_message(int sockfd, const char * name);

void send_in_room_message(int sockfd,const char * name, const char * str);