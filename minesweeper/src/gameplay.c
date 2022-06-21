
#include <graphx.h>
#include <stdlib.h>
#include <stdio.h>

#include <cutil/gameloop.h>
#include <cutil/keys.h>

#include <cutil/cgraphx.h>
#include <cutil/menu.h>

#include "cutil/misc.h"
#include "ms_misc.h"
#include "ms_styles.h"
#include "states.h"
#include "tice.h"
#include "minesweeper.h"

#include "gfx/tiles16.h"

#define FOCUSED_KEYS_LEN 9 
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Clear,
    c_8, c_4, c_5, c_6,
    c_Up, c_Left, c_Down, c_Right
};

static const ms_window_template WINDOW_TMPLT = {
    .x = 0,
    .y = align(3),

    .w_width = 20,
    .w_height = 12,

    .s_width = 8,
    .s_height = 8,

    .s_r_offset = 2,
    .s_c_offset = 6 
};

typedef struct {
    ms_game *game;
    ms_window *window;

    uint8_t redraw;
} gameplay_state;

static void *enter_gameplay(void *glb_state, void *trans_state) {
    (void)glb_state;

    gameplay_state *gp_state = safe_malloc(sizeof(gameplay_state));


    gp_state->game = new_ms_game((const ms_difficulty *)trans_state);
    gp_state->window = new_ms_window(&WINDOW_TMPLT, gp_state->game);


    cgfx_pane_nc(&PANE_STYLE_0, 0, 0, LCD_WIDTH, align(3)); 
    render_ms_window_nc(gp_state->window);
    gfx_BlitBuffer();

    gp_state->redraw = 0;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return gp_state;
}

static const loc_life_cycle *update_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    gameplay_state *gp_state = (gameplay_state *)loc_state;

    scan_focused_keys();

    if (key_press(c_Clear)) {
        return &HOMEPAGE;
    }

    gp_state->redraw |= update_ms_window(gp_state->window);

    return &GAMEPLAY;
}

static void render_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    gameplay_state *gp_state = (gameplay_state *)loc_state;

    if (!gp_state->redraw) {
        return;
    }

    render_ms_window_nc(gp_state->window);
    gfx_SwapDraw();

    gp_state->redraw = 0;
}

static void *exit_gameplay(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc;

    gameplay_state *gp_state = (gameplay_state *)loc_state;

    del_ms_window(gp_state->window);
    del_ms_game(gp_state->game);
    free(gp_state);

    return NULL;
}

const loc_life_cycle GAMEPLAY = {
    .enter = enter_gameplay,
    .update = update_gameplay,
    .render = render_gameplay,
    .exit = exit_gameplay
};
