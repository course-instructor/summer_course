
// server_handlers.h
#pragma once

#include "common.h"
#include "s_rooms.h"

/**
 * Handle user sign-up request.
 * @return 0 on success, non-zero on failure
 */
int handle_signup(const char *name, const char *password);

/**
 * Handle user login request.
 * @return new connection_status
 */

int handle_login(client_s client, const char *name, const char *password);

/**
 * Handle request for room list.
 * @return send_message result
 */
int handle_room_lst_message(client_ptr_t client);

/**
 * Handle user entering a room.
 * @return send_message result
 */
int handle_enter_room(client_ptr_t client, int room_num);
