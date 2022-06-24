
#include <graphx.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/timers.h>

#include <cutil/gameloop.h>
#include <cutil/keys.h>

#include <cutil/cgraphx.h>
#include <cutil/menu.h>

#include "cutil/data.h"
#include "cutil/misc.h"
#include "ms_misc.h"
#include "ms_styles.h"
#include "states.h"
#include "tice.h"
#include "minesweeper.h"
#include "ms_window.h"

#include "gfx/tiles16.h"

#define FOCUSED_KEYS_LEN 11
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Clear,
    c_8, c_4, c_5, c_6,
    c_Up, c_Left, c_Down, c_Right,
    c_7, c_9
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

// Information used for rendering flags left and time elapsed.
typedef struct {
    uint16_t fl_buffer, fl_screen, fl_actual; 
    uint16_t te_buffer, te_screen, te_actual;
} score_render;

static uint8_t update_score_render(score_render *sr, ms_game *game) {
    uint8_t ret = 0;

    if (sr->fl_actual != game->flags_left) {
        sr->fl_actual = game->flags_left;
        ret = 1;
    }

    if (sr->te_actual != game->time_elapsed) {
        sr->te_actual = game->time_elapsed;
        ret = 1;
    }
    
    return ret;
}

#define NUMBER_BORDER   4
#define STAT_WIDTH      (2 * NUMBER_BORDER + 3 * 8) 
#define SPACER_WIDTH    24
#define BAR_WIDTH       (SPACER_WIDTH + 2 * STAT_WIDTH)
#define BAR_HEIGHT      (2 * NUMBER_BORDER + 16)
#define BAR_START_X     ((LCD_WIDTH - BAR_WIDTH) / 2) 
#define BAR_START_Y     ((48 - BAR_HEIGHT) / 2) 

// This is very static, renders in the same place every time.
// Very little flexibility by design.
static void render_score_render(score_render *sr) {
    gfx_SetMonospaceFont(8);
    gfx_SetTextScale(1, 2);

    gfx_SetColor(1);
    gfx_SetTextFGColor(4);

    if (sr->fl_actual != sr->fl_buffer) {
        gfx_FillRectangle(BAR_START_X, BAR_START_Y, STAT_WIDTH, BAR_HEIGHT);
        gfx_SetTextXY(BAR_START_X + NUMBER_BORDER, BAR_START_Y + NUMBER_BORDER);
        gfx_PrintUInt(sr->fl_actual, 3);
    }    

    if (sr->te_actual != sr->te_buffer) {
        gfx_FillRectangle(BAR_START_X + STAT_WIDTH + SPACER_WIDTH, BAR_START_Y, 
                STAT_WIDTH, BAR_HEIGHT);
        gfx_SetTextXY(BAR_START_X + STAT_WIDTH + SPACER_WIDTH + NUMBER_BORDER, 
                BAR_START_Y + NUMBER_BORDER);
        gfx_PrintUInt(sr->te_actual, 3);
    }

    // turn off monospacing.
    gfx_SetMonospaceFont(0);

    sr->fl_buffer = sr->fl_screen;
    sr->fl_screen = sr->fl_actual;

    sr->te_buffer = sr->te_screen;
    sr->te_screen = sr->te_actual;
}

#define FLAGS_LEFT_NO_RENDER (UINT8_MAX + 1)
// 999 is max time elapsed.
#define TIME_EL_NO_RENDER (MS_TIMEOUT + 1) 

typedef struct {
    ms_window *window;
    score_render sr;
    uint8_t redraw;
} gameplay_state;

static void *enter_gameplay(void *glb_state, void *trans_state) {
    (void)glb_state;
    const ms_difficulty *diff = (const ms_difficulty *)trans_state;

    gameplay_state *gp_state = safe_malloc(sizeof(gameplay_state));

    gp_state->window = 
        new_ms_window(&WINDOW_TMPLT, diff);

    gp_state->sr.fl_actual = FLAGS_LEFT_NO_RENDER;
    gp_state->sr.te_actual = TIME_EL_NO_RENDER;

    cgfx_pane_nc(&PANE_STYLE_2, 0, 0, LCD_WIDTH, align(3)); 
    gfx_BlitBuffer();

    gp_state->redraw = 1;

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

    gp_state->redraw |= update_score_render(&(gp_state->sr), gp_state->window->game);

    gp_state->redraw |= update_ms_window(gp_state->window);

    return &GAMEPLAY;
}

static void render_gameplay(void *glb_state, void *loc_state) {
    (void)glb_state;
    gameplay_state *gp_state = (gameplay_state *)loc_state;

    if (!gp_state->redraw) {
        return;
    }

    // Must render flags left and time elapsed.
    
    render_score_render(&(gp_state->sr));
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
    free(gp_state);

    return NULL;
}

const loc_life_cycle GAMEPLAY = {
    .enter = enter_gameplay,
    .update = update_gameplay,
    .render = render_gameplay,
    .exit = exit_gameplay
};
