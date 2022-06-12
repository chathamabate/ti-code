#ifndef MS_MISC_H
#define MS_MISC_H

#include <stdint.h>

#define align(c) (c * 16)

#define render_random_bg() render_random_tile16_grid(4, 5, 1, 3)

// This funciton renders a grid of random 16 x 16 tiles which takes up the
// entire screen. p / n is the chance tile0 is drawn.
void render_random_tile16_grid(uint8_t tile0, uint8_t tile1, uint8_t p, uint8_t n);

#endif