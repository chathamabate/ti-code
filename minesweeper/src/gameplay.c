
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
#include "sys/lcd.h"
#include "tice.h"
#include "minesweeper.h"
#include "ms_window.h"
#include "ms_mem_channels.h"
#include "ms_scores.h"

#include "gfx/tiles16.h"

#define FOCUSED_KEYS_LEN 12
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Clear, c_Enter,
    c_8, c_4, c_5, c_6,
    c_Up, c_Left, c_Down, c_Right,
    c_7, c_9
};

static const ms_window_template EASY_WINDOW_TMPLT = {
    .x = 0,
    .y = align(3),

    .w_width = 20,
    .w_height = 12,

    .s_width = 8,
    .s_height = 8,

    .s_r_offset = 2,
    .s_c_offset = 6 
};

static const ms_window_template DEF_WINDOW_TMPLT = {
    .x = 0,
    .y = align(3),

    .w_width = 20,
    .w_height = 12,

    .s_width = 16,
    .s_height = 8,

    .s_r_offset = 2,
    .s_c_offset = 2 
};

#define PAUSE_LABELS_LEN 3
static const char *PAUSE_LABELS[PAUSE_LABELS_LEN] = {
    "Resume",
    "Restart",
    "Main Menu"
};

#define BTN_WIDTH align(8) 

static const text_menu_template PAUSE_MENU_TMPLT = {
    .button_height = 32,
    .button_width = BTN_WIDTH,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = PAUSE_LABELS,
    .len = PAUSE_LABELS_LEN,
    .format = MENU_VERTICAL,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = (LCD_WIDTH - BTN_WIDTH) / 2,
    .y = align(6) 
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

// Number of ticks of end animation to run.
#define END_ANIMATION_LEN 40

typedef struct {
    ms_window *window;
    basic_text_menu *pause_menu;
    score_render sr;

    // This is if we just paused.
    // Signifying a single background render and BLit
    // is needed.
    uint8_t recent_pause;

    uint8_t redraw;
} gameplay_state;

static void *enter_gameplay(void *glb_state, void *trans_state) {
    (void)glb_state;

    // Extract given difficulty index.
    uint8_t *diff_ind_ptr = (uint8_t *)trans_state;
    uint8_t diff_ind = *diff_ind_ptr;

    safe_free(GAMEPLAY_CHANNEL, diff_ind_ptr);

    gameplay_state *gp_state = safe_malloc(GAMEPLAY_CHANNEL, sizeof(gameplay_state));

    gp_state->window = new_ms_window(
            diff_ind == MS_EASY ? &EASY_WINDOW_TMPLT : &DEF_WINDOW_TMPLT, 
            diff_ind
    );

    // Initialize the first game.
    init_ms_window(gp_state->window);
    
    gp_state->pause_menu = new_basic_text_menu(&PAUSE_MENU_TMPLT, &MS_MENU_SS);
    focus_basic_text_menu(gp_state->pause_menu);

    // Initialize the status row render memory.
    gp_state->sr.fl_actual = FLAGS_LEFT_NO_RENDER;
    gp_state->sr.fl_buffer = FLAGS_LEFT_NO_RENDER;
    gp_state->sr.fl_screen = FLAGS_LEFT_NO_RENDER;

    gp_state->sr.te_actual = TIME_EL_NO_RENDER;
    gp_state->sr.te_buffer = TIME_EL_NO_RENDER;
    gp_state->sr.te_screen = TIME_EL_NO_RENDER;

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

    if (!gp_state->window->paused) {
        gp_state->redraw |= update_score_render(&(gp_state->sr), gp_state->window->game);
        gp_state->redraw |= update_ms_window(gp_state->window);

        if (gp_state->window->animation_tick > END_ANIMATION_LEN) {
            return gp_state->window->game->game_state == MS_WIN 
                ? &VICTORY : &DEFEAT;
        }

        // Check if the update triggered a pause.
        if (gp_state->window->paused) {
            gp_state->recent_pause = 1;

            // Make sure the menu is aware that none of itself
            // might be on the screen or in the buffer at this point.
            reset_render_text_menu(gp_state->pause_menu->super);
        }

        return &GAMEPLAY;
    }

    // Pause update logic.

    if (key_press(c_Enter)) {
        switch (gp_state->pause_menu->selection) {
        case 0:
            resume_ms_window(gp_state->window);
            gp_state->redraw = 1;
            return &GAMEPLAY;
        case 1:
            init_ms_window(gp_state->window);
            gp_state->redraw = 1;
            return &GAMEPLAY;
        case 2:
            return &HOMEPAGE;
        }
    }

    // update.
    gp_state->redraw |= update_basic_text_menu(gp_state->pause_menu);    

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

    if (gp_state->recent_pause) {
        // Place pause menu background in both buffer and screen.
        render_tile16_grid_nc(BG_HIDDEN(BLACK), 0, align(3), LCD_WIDTH, align(12));
        gfx_BlitRectangle(gfx_buffer, 0, align(3), LCD_WIDTH, align(12));

        gp_state->recent_pause = 0;
    }

    if (gp_state->window->paused) {
        render_text_menu_nc(gp_state->pause_menu->super);
    } else {
        // Only render when not paused.
        render_ms_window_nc(gp_state->window);
    }

    gfx_SwapDraw();

    gp_state->redraw = 0;
}

static void *exit_gameplay(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    gameplay_state *gp_state = (gameplay_state *)loc_state;

    uint8_t diff_ind = gp_state->window->game->diff_ind;
    uint16_t score = gp_state->window->game->time_elapsed;

    del_ms_window(gp_state->window);
    del_basic_text_menu(gp_state->pause_menu);
    safe_free(GAMEPLAY_CHANNEL, gp_state);

    if (next_loc_lc == &VICTORY) {
        trans_victory *tv = safe_malloc(VICTORY_CHANNEL, sizeof(trans_victory));

        tv->diff_ind = diff_ind;
        tv->score = score;

        return tv;
    }

    if (next_loc_lc == &DEFEAT) {
        uint8_t *diff_ptr = safe_malloc(DEFEAT_CHANNEL, sizeof(uint8_t));
        *diff_ptr = diff_ind;
        return diff_ptr;       
    }

    return NULL;
}

const loc_life_cycle GAMEPLAY = {
    .enter = enter_gameplay,
    .update = update_gameplay,
    .render = render_gameplay,
    .exit = exit_gameplay
};
