
// server_handlers.h
#pragma once

#include "common.h"
#include "s_rooms.h"



int process_signup(const char *name, const char *password);

int process_login( const char *name, const char *password);

message_s * handle_room_lst_message(client_ptr_t client);

message_s * handle_enter_room(const char * buf, client_s * c);

int process_enter_room(client_ptr_t c,int room_num);

int proccess_exit_room(client_s * client);