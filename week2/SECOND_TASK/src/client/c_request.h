#pragma once

#include "common.h"


/**
 * @brief the function sends signup message to the server
 *
 * @param sockfd server socket
 * @param name the name to sign up
 * @param password the password to sign up
 */
void send_signup_message(int sockfd, const char *name, const char *password);


/**
 * @brief the function sends login message to the server
 *
 * @param sockfd server socket
 * @param name the name to log in
 * @param password the password to log in
 */
void send_login_message(int sockfd, const char *name, const char *password);


/**
 * @brief  the function sends a room list message
 *
 * @param sockfd the servers socket
 */
void send_room_lst_message(int sockfd);


/**
 * @brief  the function sends a enter room request
 *
 * @param sockfd servers socket
 * @param name name to enter as
 * @param room room to enter
 */
void send_enter_room_message(int sockfd, const char* name, int room);


/**
 * @brief  the function sends leave room message to the server
 *
 * @param sockfd servers socket
 * @param name name to exit as
 */
void send_leave_room_message(int sockfd, const char * name);

/**
 * @brief  the function sends a message to the server to send to all the other clients in the room
 *
 * @param sockfd servers socket
 * @param name the name that the message will be from
 * @param str the message that will be sent to everyone in the room
 */
void send_in_room_message(int sockfd,const char * name, const char * str);