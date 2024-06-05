#ifndef BASE_H
#define BASE_H

#include <stddef.h>
#include <stdint.h>
#include <ncurses.h>
#include <stdlib.h>

#define W_PART 20
#define H_PART 15
#define W W_PART*2
#define H H_PART*2

#define MAGIC 0xabcdfe01
#define DIR_PTYPE 0x00
#define CONNECT_PTYPE 0x01
#define READY_PTYPE 0x02
#define MAP_PTYPE 0x10
#define START_PTYPE 0x20
#define SERVER_DIR_PTYPE 0xffffffff

#define WALL_BLOCK 0xff
#define MEAL_BLOCK 0xaa
#define PLAYER_BLOCK 0x22

#define putchar(x, y, sym) mvaddch(y, x, sym)


// ENUMS
enum COLORS {
    _,
    COLOR_YOU,
    COLOR_OPPONENT,
};

enum DIRECTION {
    UP,
    RIGHT,
    DOWN,
    LEFT
};


// STRUCTS
struct Player {
    uint32_t x;
    uint32_t y;
    uint32_t direction;
    char name[256];
    int sd;
    int init_stage;
};

struct Packet {
    uint32_t magic;
    uint32_t ptype;
    uint32_t datasize;
}__attribute__((packed));

struct StartPkt {
    uint32_t frame_timeout;
    uint32_t player_count;
}__attribute__((packed));

struct PlayerPkt {
    uint32_t start_x;
    uint32_t start_y;
    uint32_t start_direction;
    uint32_t player_name_len;
}__attribute__((packed));


// GLOBALS
extern int IS_SERVER;
extern int PLAYERS_COUNT;
extern int DELAY;
extern int PLAYING;
extern char* NAME;
extern int INDEX;  // current player index
extern struct Player players[4];
extern int8_t** MAP;

// FUNCTIONS
void init_colors();
void quit(int num);
void init_map();
void init_game();
int get_direction_by_key(int key);
void show_map(int8_t** map);
int index_by_player_name(char* name);
char dir_to_sym(int dir);
void next_block(int* x, int* y, int direction);
int verify_block(int x, int y);
void render_map();
void player_step(int i);
void render();

#endif
