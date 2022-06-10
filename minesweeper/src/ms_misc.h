#ifndef MS_MISC_H
#define MS_MISC_H

#include <stdint.h>

// This funciton renders a grid of random 16 x 16 tiles which takes up the
// entire screen. p / n is the chance tile0 is drawn.
void render_random_tile16_grid(uint8_t tile0, uint8_t tile1, uint8_t p, uint8_t n);

#endif