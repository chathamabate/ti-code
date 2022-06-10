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

    // Coordinates of the menu.
    uint16_t x;
    uint8_t y;

    // Number of buttons.
    // (i.e. number of entries in the labels and styles arrays)
    uint8_t len;
} text_menu_template;


// Way to deal with buffering issues....
typedef struct {
    uint8_t style;
    uint8_t screen_style;
    uint8_t buffer_style;
} buffered_styling;

typedef struct {
    const text_menu_template *template;
    uint8_t *styles;
} text_menu;

// Render a specific button of a menu.
// This is meant to be used for partial redraw.
void render_text_menu_button_nc(text_menu *menu, uint8_t i);

// Render a text menu.
void render_text_menu_nc(text_menu *menu);

// Specifications for menu rendering types.
#define NO_REDRAW       0
#define FULL_REDRAW     1
#define PARTIAL_REDRAW  2

// Basic Menu needs to be refactored...
// Focusing, and drawing mechanics should be used here...

typedef struct {
    text_menu menu;

    uint8_t selection_style;
    uint8_t deselection_style;

    // The selected button as seen in the buffer.
    // Used for partial redraw.
    uint8_t buffer_selection; 

    // The selected button as seen on the screen.
    uint8_t screen_selection;

    // The actual selection.
    uint8_t selection;

    uint8_t unfocused_style;

    // Specify if and how the menu should be redrawn.
    uint8_t redraw;
} simple_text_menu;

// Create a new simple text menu. (Starts unfocused always)
simple_text_menu *new_simple_text_menu(const text_menu_template *tmplt, 
    uint8_t s_style, uint8_t ds_style, uint8_t uf_style);

// Focus a simple text menu.
void focus_simple_text_menu(simple_text_menu *st_menu);

// Unfocus a simple text menu.
void unfocus_simple_text_menu(simple_text_menu *st_menu);

// Checks keypad input for updating selection.
// NOTE, this should only be called if the menu is focused.
// Otherwise, the rendering may not draw as expected.
void update_simple_text_menu(simple_text_menu *st_menu);

// Render the simple text menu according to its redraw specs.
// If drawing actually occurs 1 will be returned, otherwise 0.
uint8_t render_simple_text_menu_nc(simple_text_menu *st_menu);

// Delete a simple text menu.
void del_simple_text_menu(simple_text_menu *st_menu);

#endif