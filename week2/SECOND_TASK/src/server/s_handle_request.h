
// server_handlers.h
#pragma once

#include "common.h"
#include "s_rooms.h"



int process_signup(const char *name, const char *password);

int process_login( const char *name, const char *password);

int handle_room_lst_message(client_ptr_t client);

int handle_enter_room(client_ptr_t client, int room_num);
