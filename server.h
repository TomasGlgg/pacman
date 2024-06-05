#ifndef SERVER_H
#define SERVER_H

#include "base.h"

#define FIGURE_COUNT 30
#define MAX_CONNECTIONS 32


void paste_random_figure(int8_t** map);

void trace_map(int8_t** map, int x, int y);
int check_map_connectivity(int8_t** map);
void free_map(int8_t** map);
int8_t** generate_map();

void set_players();
uint8_t* convert_map();

int id_by_sd(int sd);
void send_start_packets();
void accept_clients(int sd);
void sendall_direction_packet(int player_index, int direction);
void* server_socket_handler();
void* server_keyboard_handler();

int server_main(const struct sockaddr_in addr);

#endif
