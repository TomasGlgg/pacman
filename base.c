#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

#include "base.h"


static int DELAY = 1000;
static int PLAYING = TRUE;


void init_colors() {
    if (!has_colors()) {
        endwin();
        puts("Your terminal does not support color");
        exit(1);
    }
    start_color();
    init_pair(COLOR_YOU, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_OPPONENT, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_APPLE, COLOR_GREEN, COLOR_BLACK);
}

void quit(int num) {
    PLAYING = FALSE;
    endwin();

    printf("Interrupt: %d\n", num);
    exit(0);
}

void game_init() {
    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    signal(SIGINT, quit);
    init_colors();
}

void render() {

}
