#include <sys/socket.h>
#include <arpa/inet.h>

#include "client.h"

void client_socket_handler() {

}

void recv_map() {

}

int client_main(struct sockaddr_in addr) {
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sd, &addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Connect");
        return 1;
    }

    recv_map();
    // wait user ready
    // send ready to server

    game_init();
}
