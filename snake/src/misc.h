#ifndef MISC_H
#define MISC_H

#define START_PAGE      0
#define HIGHSCORES      1
#define IN_PLAY         2
#define DEFEAT          3
#define NEW_HIGHSCORE   4
#define EXIT            5

// Colors for rendering a menu.
#define MENU_BG_COLOR   0 
#define MENU_FG_COLOR   100
#define MENU_CH_COLOR   200
#define BORDER          5

#define MENU_TXT_HEIGHT 8
#define MENU_TXT_WIDTH  4

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint8_t y;
} coord;

typedef struct {
    char *text;
    uint8_t link;   // Should be a game state.
} button;

typedef struct {
    uint8_t choice;

    // These are in pixels for fast rendering.
    uint16_t width;
    uint8_t height;

    button *buttons;
    uint8_t buttons_len;
} menu;

// Create a new menu.
menu *new_menu(button *bts, uint8_t bts_len);

// Render a menu at a given coordinate.
void render_menu(menu *m, coord c);

#endif