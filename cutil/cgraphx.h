#ifndef CGRAPHX_H
#define CGRAPHX_H

#include <graphx.h>

// Library for helpful graphics functions!

void cgfx_set_bpp(uint8_t bpp);

void cgfx_sprite(gfx_sprite_t *sprite, uint8_t bpp, uint16_t x, uint8_t y);

// Reusable Menu Code.

typedef struct {
    // Each border sprite should be a 4x4 sprite.
    // The entries in this array specify the borders
    // of the button in clockwise order starting at the
    // top left corner of the button....
    // 
    // top left, top, top right, right, bottom right... etc.
    // 
    // NOTE, the sprites that do not correspond to corners
    // will be stretched to cover the entire button.
    gfx_sprite_t *border_sprites[8];

    // This is the palette index of the color to fill the
    // center rectangle of the button.
    uint8_t fill;
} cgfx_button_style;

typedef struct {
    cgfx_button_style *normal;
    cgfx_button_style *selected;
} cgfx_menu_style;

typedef struct {
    const char **button_labels;
    uint8_t len;
    
    uint8_t selection;
} cgfx_menu;

#endif