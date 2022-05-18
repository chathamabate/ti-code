#ifndef MISC_H
#define MISC_H

// Pallete from dark to light.
#define COLOR_0 16
#define COLOR_1 20
#define COLOR_2 144
#define COLOR_3 151
#define COLOR_4 244
#define COLOR_5 247

// Investigate fonts in a later project...
//
// By default text is 8x8.
// We are just going to rescale the text.
// 
#define MENU_TXT_H_SCALE 2
#define MENU_TXT_W_SCALE 1

#define MENU_BORDER 5

#define menu_down(m) \
    (m)->choice++; \
    (m)->choice %= m->buttons_len

// Modulo will not always work here.
#define menu_up(m) \
    (m)->choice = (m)->choice ? (m)->choice - 1 : (m)->buttons_len - 1;

#define menu_link(m) (((m)->buttons[(m)->choice]).link)

#define center(width) ((LCD_WIDTH - (width)) / 2)

#include <stdint.h>

typedef struct {
    uint16_t x;
    uint8_t y;
} coord;

typedef struct {
    char *text;
    uint8_t link;           // Should be a game state.

    uint8_t _text_width;    // Don't set Manually!
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
void install_menu(menu *m, button *bts, uint8_t bts_len);

#define render_menu(m, c) render_menu_xy(m, (c).x, (c).y)

void render_menu_xy(menu *m, uint16_t x, uint8_t y);

// Aligns str to the left in buff. Fills the rest of buff with the fill
// character. lens here should not include terminator character.
void left_align(char *buff, uint16_t buff_len, char *str, uint16_t str_len, char fill);

// Same as left alight, just to the right.
void right_align(char *buff, uint16_t buff_len, char *str, uint16_t str_len, char fill);

// Get the number of base 10 digits a number has.
uint8_t dec_digits(uint16_t num);

#endif