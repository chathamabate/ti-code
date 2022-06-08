
#include <cutil/gameloop.h>
#include <cutil/misc.h>

#include "states.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

#include "gfx/ms_palette.h"
#include "gfx/tiles16.h"

// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

static void *enter_game(void) {
    set_malloc_fail_routine(graphics_malloc_fail_routine);

    gfx_Begin();
    gfx_SetDrawBuffer();

    return NULL;
}

static void exit_game(void *glb_state) {
    (void)glb_state;

    gfx_End();

    set_malloc_fail_routine(normal_malloc_fail_routine);
}

const glb_life_cycle GLOBAL_LC = {
    .enter = enter_game,
    .exit = exit_game
};

int main(void) {
    run_game(100, &GLOBAL_LC, &HOMEPAGE);

    // gfx_Begin();
    // gfx_SetPalette(ms_palette, sizeof_ms_palette, 0);
    // gfx_SetTransparentColor(0);
    // gfx_FillScreen(4);

    // uint8_t x, y;

    // for (x = 0; x < 20; x++) {
    //     for (y = 0; y < 15; y++) {
    //         uint8_t found = rand() % 10;

    //         if (found > 1) {
    //             uint8_t num = rand() % 9;

    //             gfx_Sprite(tiles16_tile_1, x * 16, y * 16);

    //             if (num > 0) {
    //                 gfx_TransparentSprite(tiles16_tiles[num + 2], x * 16, y * 16);
    //             }
                
    //             continue;
    //         }

    //         gfx_Sprite(tiles16_tile_0, x * 16, y * 16);
    //         if (found == 1) {
    //             gfx_TransparentSprite(tiles16_tile_2, x * 16, y * 16);
    //         }
    //     }
    // }

    // // gfx_TransparentSprite()
    // while (os_GetCSC() != sk_Clear);
    // gfx_End();

    return 0;
}