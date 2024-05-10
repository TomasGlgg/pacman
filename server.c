#include <sys/socket.h>
#include <arpa/inet.h>

#include "server.h"


void server_socket_handler() {

}

int check_map() {
    // https://neerc.ifmo.ru/wiki/index.php?title=Использование_обхода_в_глубину_для_проверки_связности
}

void generate_map() {
    //printf("Generating map...\n");
}

int server_main(struct sockaddr_in addr) {
    generate_map();

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) return 1;
    if (bind(sd, &addr, sizeof(addr)) < 0) {
        perror("Bind");
        return 1;
    }
    listen(sd, 1);

    game_init();
}
