
#include <graphx.h>
#include <stdlib.h>

#include <cutil/gameloop.h>
#include <cutil/keys.h>

#include <cutil/cgraphx.h>
#include <cutil/menu.h>

#include "ms_misc.h"
#include "ms_styles.h"
#include "states.h"
#include "tice.h"
#include "minesweeper.h"

#include "gfx/tiles16.h"

#define FOCUSED_KEYS_LEN 1
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Clear    
};

static void *enter_gameplay(void *glb_state, void *trans_state) {
    (void)glb_state;
    (void)trans_state;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    // NOTE... THIS IS A TEST...
    // MEMORY LEAK.
    ms_game *game = new_ms_game((const ms_difficulty *)trans_state);

    // just render the grid.
    render_tile16_grid(0);

    uint8_t r, c;

    uint16_t x;
    uint8_t y;
    
    for (r = 0, y = 0; r < game->diff->grid_height && y < LCD_HEIGHT; r++, y += 16) {
        for (c = 0, x = 0; c < game->diff->grid_width && x < LCD_WIDTH; c++, x += 16) {
            gfx_Sprite_NoClip(tiles16_tile_1, x, y);

            uint8_t t = game->board[r][c].type;

            if (t == 0) {
                continue;
            }

            gfx_TransparentSprite_NoClip(tiles16_tiles[9 + t], x, y);
        }
    }


    gfx_SwapDraw();

    return NULL;
}

static const loc_life_cycle *update_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    scan_focused_keys();

    if (key_press(c_Clear)) {
        return &HOMEPAGE;
    }

    return &GAMEPLAY;
}

static void render_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_gameplay(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc;

    return NULL;
}

const loc_life_cycle GAMEPLAY = {
    .enter = enter_gameplay,
    .update = update_gameplay,
    .render = render_gameplay,
    .exit = exit_gameplay
};
