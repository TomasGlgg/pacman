#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/time.h>

#include "server.h"
#include "client.h"


void print_help(char* bin_name) {
    printf("Usage:\n\tclient: $ %1$s -n NAME [-a ADDRESS] [-p PORT]\n\tserver: $ %1$s -l -c COUNT -n NAME [-a ADDRESS] [-p PORT] [-d DELAY]\n", bin_name);
}

int check_args() {
    return NAME != NULL && (!IS_SERVER || 2 <= PLAYERS_COUNT <= 4);
}

int main(int argc, char** argv) {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(4444);
    int opt;
    while ((opt = getopt(argc, argv, "hlcp:a:d:n:")) != -1) {
        switch (opt) {
            case 'l':
                IS_SERVER = TRUE;
                break;
            case 'c':
                PLAYERS_COUNT = atoi(optarg);
                break;
            case 'p':
                addr.sin_port = atoi(optarg);
                break;
            case 'a':
                addr.sin_addr.s_addr = inet_addr(optarg);
                break;
            case 'd':
                DELAY = atoi(optarg);
                break;
            case 'n':
                NAME = optarg;
            case 'h':
            default:
                print_help(argv[0]);
                return 0;
        }
    }

    if (!check_args()) {
        print_help(argv[0]);
        return 1;
    }

    pthread_t net_thread;
    switch (IS_SERVER) {
        case TRUE:  // server
            if (server_main(addr)) return 1;
            break;
        case FALSE:  // client
            if (client_main(addr)) return 1;
            break;
    }

    pthread_t keyboard_thread;
    //pthread_create(&keyboard_thread, NULL, keyboard_handler, NULL);
    //render();
    return 0;
}
