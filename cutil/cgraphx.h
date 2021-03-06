#ifndef CGRAPHX_H
#define CGRAPHX_H

#include <graphx.h>

#ifdef __cplusplus
extern "C" {
#endif

// Library for helpful graphics functions!

// Reusable Menu Code.

// Border directions as stored in a cgfx_pane_style.
#define P_NW    0
#define P_N     1
#define P_NE    2
#define P_E     3
#define P_SE    4
#define P_S     5
#define P_SW    6
#define P_W     7

#define P_BORDER_SPRITES_LEN 8

typedef struct {
    // The entries in this array specify the borders
    // of the button in clockwise order starting at the
    // top left corner of the button....
    // 
    // top left, top, top right, right, bottom right... etc.
    // 
    // NOTE, the sprites that do not correspond to corners
    // will be stretched to cover the entire button.
    //
    // Must be 8 border sprites.
    gfx_sprite_t **border_sprites;

    // This is the palette index of the color to fill the
    // center rectangle of the button.
    uint8_t fill;

    // Palette index of text foreground color of the pane.
    // (NOTE : this is not always used)
    uint8_t text_fg;
} cgfx_pane_style;

// Set the border tile size for rendering.
void cgfx_set_pane_border_size(uint8_t b_size);

// Fill a pane on screen.
// width and height both must be multiples of border size.
// width and height both most be greater than or equal to 2 * border size.
void cgfx_pane_nc(const cgfx_pane_style *style, uint16_t x, uint8_t y, uint16_t width, uint8_t height);

// NOTE this is a late addition to this project.
// Could be integrate more into other libraries.
// This is in this file since it pertains solely to rendering.
typedef struct {
    const cgfx_pane_style *style;

    uint16_t x;
    uint8_t y;
    uint16_t w;
    uint8_t h;

    uint8_t text_w_sc;
    uint8_t text_h_sc;
    uint8_t text_color;

    const char *text;
} text_pane_template;

void cgfx_text_pane_nc(const text_pane_template *tmplt);

#ifdef __cplusplus
}
#endif

#endif
