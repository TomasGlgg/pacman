#ifndef CLIENT_H
#define CLIENT_H

#include "base.h"

static int sd;

void client_socket_handler();
void recv_map(int sd);
int client_main(struct sockaddr_in addr);

#endif
