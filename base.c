#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "base.h"

#define msleep(msec) usleep(msec*1000)


int IS_SERVER;
int PLAYERS_COUNT = 0;
int DELAY = 1000;
int PLAYING = TRUE;
char* NAME;
int INDEX = 0;
struct Player players[4];
int8_t** MAP = NULL;


void init_colors() {
    if (!has_colors()) {
        endwin();
        puts("Your terminal does not support color");
        exit(1);
    }
    start_color();
    init_pair(COLOR_YOU, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_OPPONENT, COLOR_BLUE, COLOR_BLACK);
}

void quit(int num) {
    PLAYING = FALSE;
    endwin();

    printf("Interrupt: %d\n", num);
    exit(0);
}

void init_map() {
    MAP = malloc(W * sizeof(int8_t*));
    for (int i = 0; i<W; i++)
        MAP[i] = (int8_t*)malloc(H * sizeof(int8_t));
}

void init_game() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    signal(SIGINT, quit);
    init_colors();
}

int get_direction_by_key(int key) {
    switch (key) {
        case 's':
        case KEY_DOWN:
            return DOWN;
        case 'a':
        case KEY_LEFT:
            return LEFT;
        case 'w':
        case KEY_UP:
            return UP;
        case 'd':
        case KEY_RIGHT:
            return RIGHT;
    }
    return -1;
}

void show_map(int8_t** map) {
    for (int y = 0; y<H; y++) {
        for (int x = 0; x<W; x++) {
            if (map[x][y])
                printf("#");
            else
                printf(".");
        }
        printf("\n");
    }
    printf("\n");
}

int index_by_player_name(char* name) {
    for (int i = 0; i<PLAYERS_COUNT; i++) {
        if (!strcmp(players[i].name, name)) return i;
    }
    return -1;
}

char dir_to_sym(int dir) {
    switch (dir) {
        case UP: return '^';
        case RIGHT: return '>';
        case DOWN: return 'V';
        case LEFT: return '<';
    }
}

void next_block(int* x, int* y, int direction) {
    switch (direction) {
        case UP: (*y)--; break;
        case RIGHT: (*x)++; break;
        case DOWN: (*y)++; break;
        case LEFT: (*x)--; break;
    }
}

void render_map() {
    clear(); // clear screen
    putchar(0, 0, '+');
    putchar(W+1, 0, '+');
    putchar(0, H+1, '+');
    putchar(W+1, H+1, '+');
    for (int i = 1; i<W+1; i++) {
        putchar(i, 0, '-');
        putchar(i, H+1, '-');
    }
    for (int i = 1; i<H+1; i++) {
        putchar(0, i, '|');
        putchar(W+1, i, '|');
    }

    for (int x = 0; x<W; x++) {
        for (int y = 0; y<H; y++) {
            if (MAP[x][y] == 1) putchar(x+1, y+1, '#');
            else if (MAP[x][y] == 3) putchar(x+1, y+1, '+');
            else putchar(x+1, y+1, '.');
        }
    }

    // set players
    for (int i = 0; i<PLAYERS_COUNT; i++) {
        struct Player player = players[i];
        int color = i == INDEX ? COLOR_PAIR(COLOR_YOU) : COLOR_PAIR(COLOR_OPPONENT);
        attron(color);
        putchar(player.x + 1, player.y + 1, dir_to_sym(player.direction));
        attroff(color);
    }

    refresh();
}

int verify_block(int x, int y) {
    return x > 0 && y > 0 && x < W && y < H;
}

void player_step(int i) {
    struct Player* player = players + i;

    int next_x = player->x;
    int next_y = player->y;
    next_block(&next_x, &next_y, player->direction);
    if (verify_block(next_x, next_y)) {
        switch (MAP[next_x][next_y]) {
            case 1:  // eat
                MAP[next_x][next_y] = 3;
            case 3:  // free
                player->x = next_x;
                player->y = next_y;
        }
    }
}

void render() {
    render_map();
    msleep(DELAY);

    while (PLAYING) {

        for (int i = 0; i<PLAYERS_COUNT; i++)
            player_step(i);

        render_map();
        msleep(DELAY);

    }
}
