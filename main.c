#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>

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
    while ((opt = getopt(argc, argv, "hlc:p:a:d:n:")) != -1) {
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
                break;
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

    switch (IS_SERVER) {
        case TRUE:  // server
            if (server_main(addr)) return 1;
            break;
        case FALSE:  // client
            if (client_main(addr)) return 1;
            break;
    }

    render();
    return 0;
}
