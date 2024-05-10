#ifndef BASE_H
#define BASE_H

#include <stddef.h>
#include <stdint.h>
#include <ncurses.h>


// ENUMS
enum COLORS {
    _,
    COLOR_YOU,
    COLOR_OPPONENT,
    COLOR_APPLE
};

enum DIRECTION {
    UP,
    RIGHT,
    DOWN,
    LEFT
};

// STRUCTS
struct Player {
    uint32_t start_x;
    uint32_t start_y;
    uint32_t start_direction;
    char player_name[256];
    int sd;
};

struct PlayerPkt {
    uint32_t start_x;
    uint32_t start_y;
    uint32_t start_direction;
    uint32_t player_name_len;
    uint8_t* player_name;
}__attribute__((packed));

struct StartPkt {
    uint32_t frame_timeout;
    uint32_t player_count;
}__attribute__((packed));

// GLOBALS
static int IS_SERVER;
static int PLAYERS_COUNT;
static int DELAY;
static int PLAYING;
static char* NAME;
static struct Player players[3];

// FUNCTIONS
void init_colors();
void quit(int num);
void game_init();
void render();

#endif
