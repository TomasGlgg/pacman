#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <sys/poll.h>
#include <unistd.h>
#include <pthread.h>

#include "server.h"

int figures[4][4][2] = {
    { {0, 0}, {0, 1}, {1, 0}, {1, 1} },  // rect
    { {0, 0}, {0, 1}, {0, 2}, {1, 2} },  // L
    { {0, 1}, {1, 1}, {1, 0}, {2, 0} },
    { {0, 0}, {0, 1}, {1, 1}, {1, 2} }
};

void paste_random_figure(int8_t** map) {
    int figure_number = rand()%2;

    int x = rand() % W_PART;
    int y = rand() % H_PART;

    int x_block, y_block;
    for (int i = 0; i<4; i++) {
        x_block = x + figures[figure_number][i][0];
        y_block = y + figures[figure_number][i][1];

        if (x_block >= 0 && x_block < W_PART
            && y_block >= 0 && y_block < H_PART)
            map[x_block][y_block] = 0;

    }
}

void trace_map(int8_t** map, int x, int y) {
    if (map[x][y]) {
        map[x][y] = 0;

        if (x-1 >= 0) trace_map(map, x-1, y);
        if (x+1 < W) trace_map(map, x+1, y);
        if (y-1 >= 0) trace_map(map, x, y-1);
        if (y+1 < H) trace_map(map, x, y+1);
    }
}

int check_map_connectivity(int8_t** orig_map) {
    int start_x, start_y;
    int8_t** map = malloc(W * sizeof(int8_t*));
    for (int x = 0; x<W; x++) {
        map[x] = (int8_t*)malloc(H * sizeof(int8_t));
        for (int y = 0; y<H; y++) {
            map[x][y] = orig_map[x][y];  // copy map
            if (map[x][y]) {
                start_x = x;
                start_y = y;
            }
        }
    }

    trace_map(map, start_x, start_y);

    for (int x = 0; x<W; x++) {
        for (int y = 0; y<H; y++) {
            if (map[x][y]) return 0;
        }
    }
    return 1;
}

void free_map(int8_t** map) {
    for (int i = 0; i<W; i++)
        free(map[i]);
    free(map);
}

int8_t** generate_map() {
    int8_t** map = malloc(W * sizeof(int8_t*));
    for (int i = 0; i<W; i++)
        map[i] = (int8_t*)malloc(H * sizeof(int8_t));

    do {
        for (int x = 0; x<W; x++)
            for (int y = 0; y<H; y++) map[x][y] = 1;  // fill map

        for (int i = 0; i<FIGURE_COUNT; i++)
            paste_random_figure(map);

        for (int x = 0; x<W; x++) {
            for (int y = 0; y<H; y++) {
                map[x][H-y-1] = map[x][y];
                map[W-x-1][y] = map[x][y];
                map[W-x-1][H-y-1] = map[x][y];
            }
        }
    } while (!check_map_connectivity(map));
    return map;
}


void set_players() {
    int server_x, server_y;
    do {
        server_x = rand()%W_PART;
        server_y = rand()%H_PART;
    } while (!MAP[server_x][server_y]);
    MAP[server_x][server_y] = 2;
    players[0].x = server_x;
    players[0].y = server_y;

    int x, y;
    switch (PLAYERS_COUNT) {
        case 4:
            x = W - server_x - 1;
            y = H - server_y - 1;
            MAP[x][y] = 2;
            players[3].x = x;
            players[3].y = y;
        case 3:
            x = W - server_x - 1;
            y = server_y;
            MAP[x][y] = 2;
            players[2].x = x;
            players[2].y = y;
        case 2:
            x = server_x;
            y = H - server_y - 1;
            MAP[x][y] = 2;
            players[1].x = x;
            players[1].y = y;
    }
}

uint8_t* convert_map() {
    uint8_t* map = malloc(W_PART*H_PART);
    for (int y = 0; y<H_PART; y++) {
        for (int x = 0; x<W_PART; x++)
            switch (MAP[x][y]) {
                case 0:
                    map[y*W_PART + x] = WALL_BLOCK; break;
                case 1:
                    map[y*W_PART + x] = MEAL_BLOCK; break;
                case 2:
                    map[y*W_PART + x] = PLAYER_BLOCK; break;
            }
    }
    return map;
}

int id_by_sd(int sd) {
    for (int i = 1; i<PLAYERS_COUNT; i++) {
        if (players[i].sd == sd) return i;
    }
    return -1;
}

void send_start_packets() {
    int names_len = 0;
    for (int i = 1; i<PLAYERS_COUNT; i++)
        names_len += strlen(players[i].name);
    struct Packet pkt;
    pkt.magic = MAGIC;
    pkt.ptype = START_PTYPE;
    pkt.datasize = 4*2 + 4*8 + names_len;
    printf("Names length: %d, map packet size: %d.\n", names_len, pkt.datasize);

    struct StartPkt start_pkt;
    start_pkt.frame_timeout = DELAY;
    start_pkt.player_count = PLAYERS_COUNT;

    for (int index = 1; index<PLAYERS_COUNT; index++) {
            printf("Sending START packet to %d client.\n", index);
            send(players[index].sd, &pkt, sizeof(pkt), 0);  // header
            send(players[index].sd, &start_pkt, sizeof(start_pkt), 0);  // start pkt
    }

    for (int i = 0; i<PLAYERS_COUNT; i++) {
        struct Player player = players[i];

        struct PlayerPkt player_pkt;
        player_pkt.start_x = player.x;
        player_pkt.start_y = player.y;
        player_pkt.start_direction = player.direction;
        player_pkt.player_name_len = strlen(player.name);

        for (int index = 1; index<PLAYERS_COUNT; index++) {
            send(players[index].sd, &player_pkt, sizeof(player_pkt), 0);  // player pkt
            send(players[index].sd, &player.name, strlen(player.name), 0);  // player name
        }
    }
}

void accept_clients(int sd) {
    struct Packet map_pkt;
    map_pkt.magic = MAGIC;
    map_pkt.ptype = MAP_PTYPE;  // server send map
    map_pkt.datasize = W_PART * H_PART;
    uint8_t* map = convert_map();

    struct pollfd fds[MAX_CONNECTIONS+3] = {0};  // 3 = stdin + stdout + stderr
    fds[sd].fd = sd;
    fds[sd].events = POLLIN;
    int maxfd = sd;
    int next_player_index = 1;  // 0 - server
    int players_ready = 0;

    printf("Listening...\n");
    while (players_ready != PLAYERS_COUNT - 1) {
        int nready = poll(fds, maxfd + 1, -1);

        if (fds[sd].revents & POLLIN) {
            int cd = accept(sd, NULL, NULL);
            fds[cd].fd = cd;
            fds[cd].events = POLLIN;
            if (cd > maxfd) maxfd = cd;
            printf("New connection.\n");

            if (--nready == 0) continue;
        }

        for (int i = 0; i <= maxfd; ++i) {
            if (fds[i].revents & POLLIN) {
                struct Packet in_pkt;
                int ret = recv(i, &in_pkt, sizeof(in_pkt), 0);

                int id = id_by_sd(i);

                if(ret == 0 || in_pkt.magic != MAGIC) {  // connection closed
                close:
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);
                    //if (id != -1) {
                    //    if (players[id].init_stage == 2) players_ready--;
                    //    players[id].sd = 0;
                    //    players[id].init_stage = 0;
                    //}
                    printf("Connection closed.\n");
                    continue;
                }

                switch (in_pkt.ptype) {
                    case CONNECT_PTYPE:
                        printf("CONNECT_TYPE recived, next player index: %d, id: %d.\n", next_player_index, id);
                        if (in_pkt.datasize > 255 || in_pkt.datasize < 1 || id != -1) goto close;

                        char name[256];
                        recv(i, &name, in_pkt.datasize, 0);
                        name[in_pkt.datasize] = '\0';
                        if (index_by_player_name(name) != -1) goto close;

                        players[next_player_index].init_stage = 1;
                        players[next_player_index].sd = i;
                        strcpy(players[next_player_index].name, name);
                        players[next_player_index].name[in_pkt.datasize] = '\0';


                        send(i, &map_pkt, sizeof(map_pkt), 0);
                        send(i, map, W_PART*H_PART, 0);
                        printf("New player, index: %d, name: %s.\n", next_player_index, name);

                        next_player_index++;
                        break;
                    case READY_PTYPE:
                        printf("READY_TYPE received, index: %d.\n", id);
                        if (id == -1 || players[id].init_stage != 1) goto close;

                        players[id].init_stage = 2;
                        players_ready++;
                        break;
                }
            }
        }
    }
    close(sd);
    printf("Clients accepted.\n");
    free(map);
}

void sendall_direction_packet(int player_index, int direction) {
    struct Packet pkt;
    pkt.magic = MAGIC;
    pkt.ptype = SERVER_DIR_PTYPE;
    pkt.datasize = strlen(players[player_index].name) + 1;
    int8_t direction_ = direction;

    for (int i = 1; i<PLAYERS_COUNT; i++) {
        if (i == player_index) continue;
        int sd = players[i].sd;
        send(sd, &pkt, sizeof(pkt), 0);
        send(sd, &direction_, 1, 0);
        send(sd, players[i].name, strlen(players[i].name), 0);
    }
}

void* server_socket_handler() {
    struct pollfd fds[3] = {0};
    for (int i = 1; i<PLAYERS_COUNT; i++) {
        fds[i-1].fd = players[i].sd;
        fds[i-1].events = POLLIN;
    }

    while (PLAYING) {
        poll(fds, PLAYERS_COUNT, -1);
        for (int i = 0; i <= PLAYERS_COUNT; ++i) {
            if (fds[i].revents & POLLIN) {
                struct Packet pkt;
                int ret = recv(i, &pkt, sizeof(pkt), 0);

                if(ret == 0) {  // connection closed
                    fds[i].fd = -1;
                    fds[i].events = 0;
                    close(i);
                    printf("Connection closed.\n");
                    continue;
                }

                int sd = fds[i].fd;
                int index = i + 1;
                if (pkt.magic != MAGIC || pkt.ptype != DIR_PTYPE) continue;
                int8_t dir;
                recv(sd, &pkt, 1, 0);
                players[index].direction = dir;
                sendall_direction_packet(index, dir);
                printf("New direction for %d, value: %d.\n", index, dir);
            }
        }
    }
}

void* server_keyboard_handler() {
    int key;
    int8_t direction;
    while (PLAYING) {
        key = getch();
        direction = get_direction_by_key(key);
        if (direction == -1) continue;
        players[0].direction = direction;
        sendall_direction_packet(0, direction);
    }
}

int server_main(struct sockaddr_in addr) {
    strcpy(players[0].name, NAME);

    srand(time(NULL));

    char done;
    do {
        if (MAP != NULL) {
            printf("\n");
            free_map(MAP);
        }
        MAP = generate_map();
        show_map(MAP);
        printf("Regenerate map? [y/n]: ");
        scanf("%c%*c", &done);
    } while (done == 'y');
    set_players();

    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) return 1;
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return 1;
    }
    listen(sd, 1);
    accept_clients(sd);
    send_start_packets();

    init_game();
    pthread_t server_thread;
    pthread_create(&server_thread, NULL, server_socket_handler, NULL);
    pthread_t keyboard_thread;
    pthread_create(&keyboard_thread, NULL, server_keyboard_handler, NULL);
    return 0;
}
