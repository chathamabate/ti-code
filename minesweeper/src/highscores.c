#include "graphx.h"
#include "states.h"
#include <cutil/keys.h>
#include <cutil/menu.h>
#include "minesweeper.h"
#include "ms_styles.h"
#include "ms_misc.h"
#include "ms_scores.h"

#define FOCUSED_KEYS_LEN 5

static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Up, c_Down,
    c_8, c_5,
    c_Enter
};

#define DIFFS_LABELS_LEN 3
static const char *DIFFS_LABELS[DIFFS_LABELS_LEN] = {
    "Easy", "Medium", "Hard"    
};

static const text_menu_template DIFFS_TMPLT = {
    .button_height = align(2), 
    .button_width = align(6),
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = DIFFS_LABELS,
    .len = DIFFS_LABELS_LEN,
    .format = MENU_VERTICAL,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = align(3),
    .y = align(2)
};

#define SC_TXT_W_SCALE      1
#define SC_TXT_H_SCALE      1
#define V_SPACER            2
#define ROW_HEIGHT          ((8 * SC_TXT_H_SCALE) + V_SPACER)
#define ROW_CHARS           8

// NOTE, this renderer is assuming MS_SCORES_LEN < 10.
static void render_score_list(const slide_pane_template *tmplt,
        uint16_t *scores, uint8_t scores_len) {
    uint16_t list_width = 8 * SC_TXT_W_SCALE * ROW_CHARS;
    uint8_t list_height = 8 * SC_TXT_H_SCALE * scores_len +
        V_SPACER * (scores_len - 1);

    uint16_t x = slide_pane_center_x(tmplt, list_width);
    uint8_t y = slide_pane_center_y(tmplt, list_height);

    gfx_SetTextFGColor(1);
    gfx_SetMonospaceFont(SC_TXT_W_SCALE * 8);
    gfx_SetTextScale(SC_TXT_W_SCALE, SC_TXT_H_SCALE);

    uint8_t s_i;
    for (s_i = 0; s_i < scores_len; s_i++, y += ROW_HEIGHT) {
        gfx_SetTextXY(x, y);
        gfx_PrintUInt(s_i + 1, 1);
        gfx_PrintString(".");
        gfx_SetTextXY(x + (ROW_CHARS - 3) * SC_TXT_W_SCALE * 8, y);
        gfx_PrintUInt(scores[s_i], 3);
    }   

    gfx_SetMonospaceFont(0);
} 

static void render_easy_scores(const slide_pane_template *tmplt, void *data) {
    ms_scoreboard *sb = (ms_scoreboard *)data;
    render_score_list(tmplt, sb->scores[MS_EASY], MS_SCORES_LEN);
}

static void render_medium_scores(const slide_pane_template *tmplt, void *data) {
    ms_scoreboard *sb = (ms_scoreboard *)data;
    render_score_list(tmplt, sb->scores[MS_MEDIUM], MS_SCORES_LEN);
}

static void render_hard_scores(const slide_pane_template *tmplt, void *data) {
    ms_scoreboard *sb = (ms_scoreboard *)data;
    render_score_list(tmplt, sb->scores[MS_HARD], MS_SCORES_LEN);
}

#define SCORE_RENDERERS_LEN 3
const slide_renderer SCORE_RENDERERS[SCORE_RENDERERS_LEN] = {
    render_easy_scores,
    render_medium_scores,
    render_hard_scores
};

const slide_pane_template SCORES_TMPLT = {
    .pane_width = align(7),
    .pane_height = align(6), 
    .x = align(10),
    .y = align(2),
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .
};

static void *enter_highscores(void *glb_state, void *trans_state) {
    (void)glb_state;
    (void)trans_state;
    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return NULL;
}

static const loc_life_cycle *update_highscores(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}

static void render_highscores(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_highscores(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc;

    return NULL;
}

const loc_life_cycle HIGHSCORES = {
    .enter = enter_highscores,
    .update = update_highscores,
    .render = render_highscores,
    .exit = exit_highscores
};
