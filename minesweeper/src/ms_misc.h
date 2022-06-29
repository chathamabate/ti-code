#ifndef MS_MISC_H
#define MS_MISC_H

#include <stdint.h>
#include <tice.h>

#define align(c) (c * 16)
#define align_horizontal(w) ((20 - (w)) * 16 / 2)

#define render_random_bg() render_random_tile16_grid(6, 7, 1, 3)

// This funciton renders a grid of random 16 x 16 tiles which takes up the
// entire screen. p / n is the chance tile0 is drawn.
void render_random_tile16_grid(uint8_t tile0, uint8_t tile1, uint8_t p, uint8_t n);

#define render_tile16_grid(tile) render_tile16_grid_nc(tile, 0, 0, LCD_WIDTH, LCD_HEIGHT)

// Fill a portion of the screen with a single tile
void render_tile16_grid_nc(uint8_t tile, uint16_t x, uint8_t y, uint16_t g_width, uint8_t g_height);

#endif
