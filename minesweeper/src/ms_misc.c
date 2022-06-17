#include "ms_misc.h"

#include <stdint.h>
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

void render_tile16_grid_nc(uint8_t tile, uint16_t x, uint8_t y, 
        uint16_t g_width, uint8_t g_height) {
    uint16_t x_p;
    uint8_t y_p;

   for (x_p = x; x_p < x + g_width; x_p += 16) {
       for (y_p = y; y_p < y + g_height; y_p += 16) {
           gfx_Sprite_NoClip(tiles16_tiles[tile], x_p, y_p);
       }
   }
}
