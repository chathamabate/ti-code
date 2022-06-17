
#include <graphx.h>

#include <cutil/gameloop.h>
#include <cutil/keys.h>

#include "cutil/cgraphx.h"
#include "cutil/menu.h"
#include "minesweeper/src/ms_misc.h"
#include "minesweeper/src/ms_styles.h"
#include "states.h"
#include "tice.h"

#define FOCUSED_KEYS_LEN 1
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Clear    
};

static void *enter_gameplay(void *glb_state, void *trans_state) {
    (void)glb_state;
    (void)trans_state;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);


    render_tile16_grid(4);
    render_tile16_grid_nc(0, align(2), align(5), align(16), align(8));
    cgfx_pane_nc(&PANE_STYLE_2, 0, 0, LCD_WIDTH, align(3));
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
