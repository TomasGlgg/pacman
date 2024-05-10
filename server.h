#ifndef SERVER_H
#define SERVER_H

#include "base.h"

void server_socket_handler();
int check_map();
void generate_map();
int server_main(struct sockaddr_in addr);

#endif
