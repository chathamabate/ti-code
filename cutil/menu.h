#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <cutil/cgraphx.h>

#define MENU_VERTICAL   0
#define MENU_HORIZONTAL 1

typedef struct {
    const cgfx_pane_style **style_palette;
    uint8_t style_palette_len;

    // Horizontal | Vertical
    uint8_t format;

    // Label for each button.
    const char **labels;

    // Button and label sizings.
    uint16_t button_width;
    uint8_t button_height;

    // Label will be as centered as possible.
    // Make sure scale is not too big.
    uint8_t label_width_scale;
    uint8_t label_height_scale;

    // Number of buttons.
    // (i.e. number of entries in the labels and styles arrays)
    uint8_t len;
} text_menu_template;

typedef struct {
    const text_menu_template *template;
    uint8_t *styles;
} text_menu;

// Render a specific button of a menu.
// This is meant to be used for partial redraw.
void render_text_menu_button_nc(text_menu *menu, uint16_t x, uint8_t y, uint8_t i);

// Render a text menu.
void render_text_menu_nc(text_menu *menu, uint16_t x, uint8_t y);

typedef struct {
    text_menu menu;

    uint8_t selection_style;
    uint8_t deselection_style;

    uint8_t selection;
} basic_text_menu;

// Create a new basic text menu.
basic_text_menu *new_basic_text_menu(const text_menu_template *tmplt, uint8_t s_style, uint8_t ds_style);

// Read keypad input to update the menu.
void update_basic_text_menu(basic_text_menu *bt_menu);

// Delete a basic text menu.
void del_basic_text_menu(basic_text_menu *bt_menu);

#endif