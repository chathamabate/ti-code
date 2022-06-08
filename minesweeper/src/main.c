
#include <cutil/gameloop.h>
#include <cutil/misc.h>
#include <cutil/cgraphx.h>

#include "states.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

#include <cutil/menu.h>

#include "gfx/ms_palette.h"
#include "gfx/tiles16.h"
#include "gfx/borders.h"

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
    // run_game(100, &GLOBAL_LC, &HOMEPAGE);

    cgfx_pane_style ps;

    uint8_t i;
    for (i = 0; i < 8; i++) {
        ps.border_sprites[i] = borders_tiles[i];
    }

    ps.fill = 0x07;
    ps.text_fg = 1;

    cgfx_pane_style *style_palette[] = {
        &ps
    };

    const char *labels[] = {
        "Play",
        "Highscores",
        "Instructions",
        "Exit"
    };

    uint8_t styles[] = {
        0, 0, 0, 0
    };


    text_menu tm = {
        .button_height = 20,
        .button_width = 128,
        .format = MENU_VERTICAL,
        .label_height_scale = 1,
        .label_width_scale = 1,
        .labels = labels,
        .len = 4,
        .style_palette = style_palette,
        .style_palette_len = 1,
        .styles = styles
    };

    gfx_Begin();
    gfx_SetPalette(ms_palette, sizeof_ms_palette, 0);
    gfx_SetTransparentColor(0);
    gfx_FillScreen(4);

    // render_text_menu_button_nc(&tm, 0, 0, 0);
    render_text_menu_nc(&tm, (LCD_WIDTH - tm.button_width) / 2, 64);

    // gfx_ScaledSprite_NoClip(borders_tile_0, 10, 10, 16, 16);

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

    // gfx_TransparentSprite()
    while (os_GetCSC() != sk_Clear);
    gfx_End();

    return 0;
}