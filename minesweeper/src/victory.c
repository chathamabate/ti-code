#include "cutil/cgraphx.h"
#include "cutil/misc.h"
#include <cutil/keys.h>

#include "graphx.h"
#include "ms_scores.h"
#include "states.h"

#include <stdio.h>
#include <cutil/menu.h>
#include "ms_misc.h"
#include "ms_styles.h"
#include "ms_mem_channels.h"
#include "ms_window.h"
#include "sys/lcd.h"

#define FOCUSED_KEYS_LEN 5
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Up, c_Down, 
    c_8, c_5,
    c_Enter
};

#define NAV_LABELS_LEN 2
static const char *NAV_LABELS[NAV_LABELS_LEN] = {
    "Play Again",
    "Main Menu"
};

static const text_menu_template NAV_TMPLT = {
    .button_height = align(2),
    .button_width = align(8),
    .x = align_horizontal(8),
    .y = align(10),
    .labels = NAV_LABELS,
    .len = NAV_LABELS_LEN,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .format = MENU_VERTICAL,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN
};

// Data is a pointer to the score to be displayed.
static void render_score(const slide_pane_template *tmplt, void *data) {
    char buff[20];    // Arbitrary buffer amount.
    sprintf(buff, "Score: %d", *(uint16_t *)data);
    
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(1);

    gfx_PrintStringXY(buff, 
            slide_pane_center_text(tmplt, buff), 
            slide_pane_center_y(tmplt, 8));
}

#define SCORE_RNDRRS_LEN 1
static const slide_renderer SCORE_RNDRRS[SCORE_RNDRRS_LEN] = {
    render_score
};

// Only to be used when a highscore is achieved.
static const slide_pane_template SCORE_TMPLT = {
    .pane_height = align(2),
    .pane_width = align(8),
    .x = align_horizontal(8),
    .y = align(1),
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .slide_renderers = SCORE_RNDRRS,
    .len = SCORE_RNDRRS_LEN
};

static const text_pane_template TITLE_TMPLT = {
    .x = align(5),
    .y = align(4),
    .w = align(10),
    .h = align(4),

    .text_w_sc = 2,
    .text_h_sc = 4,
    .style = &PANE_STYLE_0,
    .text_color = 1,

    .text = "Victory!"
};

typedef struct {
    basic_text_menu *nav;
    slide_pane *score_pane;

    uint8_t diff_ind;
    uint16_t score;

    uint8_t animation_tick;
    uint8_t new_hs;

    uint8_t redraw;
} victory_state;

static void *enter_victory(void *glb_state, void *trans_state) {
    ms_scoreboard *sb = (ms_scoreboard *)glb_state;
    trans_victory *tv = (trans_victory *)trans_state;
    
    victory_state *vs = safe_malloc(VICTORY_CHANNEL, sizeof(victory_state));

    vs->score = tv->score;
    vs->diff_ind = tv->diff_ind;
    safe_free(VICTORY_CHANNEL, tv);

    vs->nav = new_basic_text_menu(&NAV_TMPLT, &MS_MENU_SS);
    focus_basic_text_menu(vs->nav);

    const render INIT_RENDER = {
        .bg_style = LIGHT_BLUE,
        .fg_style = 0
    };

    vs->score_pane = new_slide_pane(&SCORE_TMPLT, INIT_RENDER);

    // Figure out whether or not new highscore has come about.
    vs->new_hs = insert_score(sb, vs->diff_ind, vs->score) != MS_SCORES_LEN;
    vs->animation_tick = 0;

    // Background Render....

    render_random_bg();
    cgfx_text_pane_nc(&TITLE_TMPLT);
    gfx_Blit(gfx_buffer);

    vs->redraw = 1;
     
    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return vs;
}

#define NEW_HS_ANIMATION_DELAY 10

static const loc_life_cycle *update_victory(void *glb_state, void *loc_state) {
    (void)glb_state;

    victory_state *vs = (victory_state *)loc_state;

    scan_focused_keys();

    if (key_press(c_Enter)) {
        return vs->nav->selection == 0 ? &GAMEPLAY : &HOMEPAGE;
    }

    if (vs->new_hs && ++vs->animation_tick % NEW_HS_ANIMATION_DELAY == 0) {
        vs->score_pane->slide.actual.bg_style = 
            vs->score_pane->slide.actual.bg_style == LIGHT_BLUE 
            ? GOLD : LIGHT_BLUE;

        vs->animation_tick = 0;
        vs->redraw = 1;
    }

    vs->redraw |= update_basic_text_menu(vs->nav);

    return &VICTORY;
}

static void render_victory(void *glb_state, void *loc_state) {
    (void)glb_state;

    victory_state *vs = (victory_state *)loc_state;

    if (!vs->redraw) {
        return;
    }


    render_slide_pane_nc(vs->score_pane, &(vs->score));
    render_text_menu_nc(vs->nav->super);

    gfx_SwapDraw();

    vs->redraw = 0;
}

static void *exit_victory(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;

    victory_state *vs = (victory_state *)loc_state;

    uint8_t diff_ind = vs->diff_ind;

    del_basic_text_menu(vs->nav);
    del_slide_pane(vs->score_pane);
    safe_free(VICTORY_CHANNEL, vs);

    if (next_loc_lc == &GAMEPLAY) {
        uint8_t *ts = safe_malloc(GAMEPLAY_CHANNEL, sizeof(uint8_t));
        *ts = diff_ind;
        return ts;
    }

    // Homepage has no transition state.
    return NULL;
}

const loc_life_cycle VICTORY = {
    .enter = enter_victory,
    .update = update_victory,
    .render = render_victory,
    .exit = exit_victory
};
