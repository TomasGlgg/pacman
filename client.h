#ifndef CLIENT_H
#define CLIENT_H

#include "base.h"

static int sd;

void* client_keyboard_handler();
void* client_socket_handler();
int receive_map();
int receive_players();
int client_main(struct sockaddr_in addr);

#endif
