#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#include "client.h"

void* client_keyboard_handler() {
    int key;
    int8_t direction;
    struct Packet pkt;
    pkt.magic = MAGIC;
    pkt.ptype = DIR_PTYPE;
    while (PLAYING) {
        key = getch();
        direction = get_direction_by_key(key);
        if (direction == -1) continue;

        players[INDEX].direction = direction;
        send(sd, &pkt, sizeof(pkt), 0);
        send(sd, &direction, 1, 0);
    }
}

void* client_socket_handler() {
    int8_t direction;
    char name[256];
    struct Packet pkt;
    while (PLAYING) {
        int res = recv(sd, &pkt, sizeof(pkt), 0);
        if (!res) quit(1);
        if (pkt.magic != MAGIC || pkt.ptype != SERVER_DIR_PTYPE) continue;

        recv(sd, &direction, 1, 0);

        int name_len = pkt.datasize - 1;
        if (name_len > 255) continue;
        recv(sd, name, name_len, 0);
        name[name_len] = '\0';
        int i = index_by_player_name(name);
        printf("Name: %s, index: %d, direction: %d.\n", name, i, direction);
        if (i < 0) continue;
        players[i].direction = direction;
    }
}

int receive_map() {
    init_map();
    struct Packet pkt;
    recv(sd, &pkt, sizeof(pkt), 0);
    if (pkt.magic != MAGIC || pkt.ptype != MAP_PTYPE) return 1;
    uint8_t map[W_PART * H_PART];
    recv(sd, &map, W_PART * H_PART, 0);

    for (int x = 0; x<W_PART; x++) {
        for (int y = 0; y<H_PART; y++) {
            switch (map[y*W_PART + x]) {
                case WALL_BLOCK:
                    MAP[x][y] = 0; break;
                case MEAL_BLOCK:
                case PLAYER_BLOCK:
                    MAP[x][y] = 1; break;
            }
            MAP[x][H-y-1] = MAP[x][y];
            MAP[W-x-1][y] = MAP[x][y];
            MAP[W-x-1][H-y-1] = MAP[x][y];
        }
    }
    show_map(MAP);
    return 0;
}

int receive_players() {
    struct Packet pkt;
    recv(sd, &pkt, sizeof(pkt), 0);
    if (pkt.magic != MAGIC || pkt.ptype != START_PTYPE) return 1;

    struct StartPkt start_pkt;
    recv(sd, &start_pkt, sizeof(start_pkt), 0);
    DELAY = start_pkt.frame_timeout;
    PLAYERS_COUNT = start_pkt.player_count;
    printf("Delay: %dms, player count: %d.\n", DELAY, start_pkt.player_count);
    for (int i = 0; i<start_pkt.player_count; i++) {
        struct PlayerPkt player_pkt;
        recv(sd, &player_pkt, sizeof(player_pkt), 0);
        if (player_pkt.player_name_len > 255) return 1;
        struct Player* player = players + i;
        player->x = player_pkt.start_x;
        player->y = player_pkt.start_y;
        player->direction = player_pkt.start_direction;
        recv(sd, player->name, player_pkt.player_name_len, 0);
        player->name[player_pkt.player_name_len] = '\0';
        printf("Index: %d, x: %d, y: %d, direction: %d, name: %s\n", i, player->x, player->y, player->direction, player->name);
        if (!strcmp(NAME, player->name)) INDEX = i;
    }
    printf("Current player index: %d\n", INDEX);
    if (INDEX == -1) return 1;
    return 0;
}

int client_main(struct sockaddr_in addr) {
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        perror("Connect");
        return 1;
    }

    struct Packet pkt;
    pkt.magic = MAGIC;
    pkt.ptype = CONNECT_PTYPE;
    pkt.datasize = strlen(NAME);
    send(sd, &pkt, sizeof(pkt), 0);
    send(sd, NAME, pkt.datasize, 0);

    if (receive_map()) return 1;

    printf("Connected, press enter to start...");
    scanf("%*c");

    pkt.ptype = READY_PTYPE;
    pkt.datasize = 0;
    send(sd, &pkt, sizeof(pkt), 0);  // send ready

    if (receive_players()) return 1;

    init_game();

    pthread_t keyboard_thread;
    pthread_create(&keyboard_thread, NULL, client_keyboard_handler, NULL);

    pthread_t client_thread;
    pthread_create(&client_thread, NULL, client_socket_handler, NULL);
}
