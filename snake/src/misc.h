#ifndef MISC_H
#define MISC_H

#define START_PAGE      0
#define HIGHSCORES      1
#define IN_PLAY         2
#define DEFEAT          3
#define NEW_HIGHSCORE   4
#define EXIT            5

// Colors for rendering a menu.
#define MENU_BG_COLOR   22 
#define MENU_FG_COLOR   17
#define MENU_CH_COLOR   240
#define MENU_BORDER     5

// Investigate fonts in a later project...
//
// By default text is 8x8.
// We are just going to rescale the text.
// 
#define MENU_TXT_H_SCALE 2
#define MENU_TXT_W_SCALE 1

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint8_t y;
} coord;

typedef struct {
    char *text;
    uint8_t text_width;
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

// Aligns str to the left in buff. Fills the rest of buff with the fill
// character. lens here should not include terminator character.
void left_align(char *buff, uint16_t buff_len, char *str, uint16_t str_len, char fill);

// Same as left alight, just to the right.
void right_align(char *buff, uint16_t buff_len, char *str, uint16_t str_len, char fill);

// Get the number of base 10 digits a number has.
uint8_t dec_digits(uint16_t num);

#endif