#include "ms_misc.h"

#include <tice.h>
#include <graphx.h>

#include "gfx/tiles16.h"

void render_random_tile16_grid(uint8_t tile0, uint8_t tile1, uint8_t p, uint8_t n) {
    uint16_t x;
    uint8_t y;

    for (x = 0; x < LCD_WIDTH; x += 16) {
        for (y = 0; y < LCD_HEIGHT; y += 16) {
            gfx_Sprite_NoClip(
                tiles16_tiles[
                    random() % n < p ? tile0 : tile1
                ],
                x, y
            );
        }
    }
}