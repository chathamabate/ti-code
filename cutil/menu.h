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

// This signifies that a button is yet to exist on
// the screen or in the buffer.
#define NOT_RENDERED 255

// Way to deal with buffering issues.
typedef struct {
    uint8_t style;
    uint8_t screen_style;
    uint8_t buffer_style;
} buffered_styling;

typedef struct {
    const text_menu_template *template;

    // The style states of each button.
    buffered_styling *styles;
} text_menu;

// Create a new text menu.
text_menu *new_text_menu(const text_menu_template *tmplt, uint8_t style);

// Render a text menu.
// NOTE, this does not always render the entire menu.
// This function will only draw buttons whose styles
// differ from that which exists in the buffer.
void render_text_menu_nc(text_menu *menu);

// delete a text menu.
void del_text_menu(text_menu *menu);

typedef struct {
    uint8_t deselection_style;
    uint8_t unfocus_style;
    uint8_t selection_style;
} selection_styling;

typedef struct {
    text_menu *super;

    // How to style selections.
    const selection_styling *ss;
    
    // The actual selection.
    uint8_t selection;
} basic_text_menu;

// Create a new basic text menu.
basic_text_menu *new_basic_text_menu(const text_menu_template *tmplt, const selection_styling *ss);

// Focus and unfocus a basic text menu.
void focus_basic_text_menu(basic_text_menu *bt_menu);
void unfocus_basic_text_menu(basic_text_menu *bt_menu);

// Checks keypad to see if selection has changed.
// Returns 1 if selection changes, 0 otherwise.
uint8_t update_basic_text_menu(basic_text_menu *bt_menu);

// Delete a basic text menu.
void del_basic_text_menu(basic_text_menu *bt_menu);

typedef struct {
    text_menu *super;
    const selection_styling *ss;

    uint8_t toggle;
    uint8_t selection;
} toggle_text_menu;

// create a new toggle menu.
toggle_text_menu *new_toggle_text_menu(const text_menu_template *tmplt, const selection_styling *ss);

// Focus and unfocus a toggle text menu.
void focus_toggle_text_menu(toggle_text_menu *tt_menu);
void unfocus_toggle_text_menu(toggle_text_menu *tt_menu);

// Update a toggle text menu.
// Returns 1 if there's a change, 0 otherwise.
uint8_t update_toggle_text_menu(toggle_text_menu *tt_menu);

// Delete a toggle menu.
void del_toggle_text_menu(toggle_text_menu *tt_menu);

// slide pane code.
// This is a pane.

#endif