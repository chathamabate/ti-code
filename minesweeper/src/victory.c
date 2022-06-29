#include "cutil/misc.h"
#include "graphx.h"
#include "ms_scores.h"
#include "states.h"

#include <stdio.h>
#include <cutil/menu.h>
#include "ms_misc.h"
#include "ms_styles.h"
#include "ms_mem_channels.h"

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
    .format = MENU_HORIZONTAL,
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
const slide_pane_template SCORE_TMPLT = {
    .pane_height = align(2),
    .pane_width = align(8),
    .x = align_horizontal(8),
    .y = align(7),
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .slide_renderers = SCORE_RNDRRS,
    .len = SCORE_RNDRRS_LEN
};

typedef struct {
    basic_text_menu *nav;
    slide_pane *score_pane;

    uint8_t diff_ind;
    uint16_t score;

    uint8_t new_hs;

    uint8_t redraw;
} victory_state;

static void *enter_victory(void *glb_state, void *trans_state) {
    ms_scoreboard *sb = (ms_scoreboard *)glb_state;
    trans_victory *tv = (trans_victory *)trans_state;
    
    victory_state *vs = safe_malloc(VICTORY_CHANNEL, sizeof(victory_state));

    vs->score = tv->score;
    vs->diff_ind = tv->diff_id;
    safe_free(VICTORY_CHANNEL, tv);

    return vs;
}

static const loc_life_cycle *update_victory(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}

static void render_victory(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_victory(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc;

    return NULL;
}

const loc_life_cycle VICTORY = {
    .enter = enter_victory,
    .update = update_victory,
    .render = render_victory,
    .exit = exit_victory
};
