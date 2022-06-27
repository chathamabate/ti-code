#include "cutil/misc.h"
#include "graphx.h"
#include "states.h"
#include <cutil/keys.h>
#include <cutil/menu.h>

#include "ms_mem_channels.h"
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

        if (scores[s_i] == MS_TIMEOUT + 1) {
            gfx_PrintString("N/A");
        } else {
            gfx_PrintUInt(scores[s_i], 3);
        }
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

const char *SELECT_MSG = "Select Difficulty";

static void render_select_diff(const slide_pane_template *tmplt, void *data) {
    (void)data;

    gfx_SetTextFGColor(1);
    gfx_SetTextScale(SC_TXT_W_SCALE, SC_TXT_H_SCALE);
    gfx_PrintStringXY(SELECT_MSG, 
            slide_pane_center_text(tmplt, SELECT_MSG),
            slide_pane_center_y(tmplt, 8 * SC_TXT_H_SCALE)
    );
}

#define SCORE_RENDERERS_LEN 4
const slide_renderer SCORE_RENDERERS[SCORE_RENDERERS_LEN] = {
    render_easy_scores,
    render_medium_scores,
    render_hard_scores,
    render_select_diff
};

#define SELECT_DIFF_FG (SCORE_RENDERERS_LEN - 1)

const slide_pane_template SCORES_TMPLT = {
    .pane_width = align(7),
    .pane_height = align(6), 
    .x = align(10),
    .y = align(2),
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .slide_renderers = SCORE_RENDERERS,
    .len = SCORE_RENDERERS_LEN 
};

#define NAV_MENU_LABELS_LEN 1
const char *NAV_MENU_LABELS[NAV_MENU_LABELS_LEN] = {
    "Back"
};

const text_menu_template NAV_MENU_TMPLT = {
    .button_height = align(2),
    .button_width = align(6),
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = NAV_MENU_LABELS,
    .len = NAV_MENU_LABELS_LEN,
    .format = MENU_HORIZONTAL,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = align(7),
    .y = align(11)
};

#define DIFFS_MENU_ID   0
#define NAV_MENU_ID     1

typedef struct {
    slide_pane *scores_pane;
    basic_text_menu *diff_menu;
    basic_text_menu *nav_menu; // NOTE this only has one button
                               // maybe in the future we will add more.

    uint8_t focused_menu;
    uint8_t redraw;
} highscores_state;

static void *enter_highscores(void *glb_state, void *trans_state) {
    (void)glb_state;
    (void)trans_state;
    
    highscores_state *hss = 
        safe_malloc(HIGHSCORES_CHANNEL, sizeof(highscores_state));

    const render INIT_RENDER = {
        .bg_style = LIGHT_BLUE,
        .fg_style = MS_EASY
    };

    hss->scores_pane = new_slide_pane(&SCORES_TMPLT, INIT_RENDER);
    hss->diff_menu = new_basic_text_menu(&DIFFS_TMPLT, &MS_MENU_SS);
    hss->nav_menu = new_basic_text_menu(&NAV_MENU_TMPLT, &MS_MENU_SS);

    // Start with difficulties focused.
    focus_basic_text_menu(hss->diff_menu);
    hss->focused_menu = DIFFS_MENU_ID;

    // Draw initial background in both buffer and screen.
    render_random_bg();
    gfx_Blit(gfx_buffer);

    hss->redraw = 1;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return hss;
}

static const loc_life_cycle *update_highscores(void *glb_state, void *loc_state) {
    (void)glb_state;

    highscores_state *hss = (highscores_state *)loc_state;

    scan_focused_keys();

    if (hss->focused_menu == NAV_MENU_ID) {
        if (key_press(c_Enter)) {
            return &HOMEPAGE;
        }

        if (key_press(c_Up) || key_press(c_8)) {
            unfocus_basic_text_menu(hss->nav_menu); 

            focus_basic_text_menu(hss->diff_menu);
            hss->scores_pane->slide.actual.bg_style = LIGHT_BLUE;
            hss->scores_pane->slide.actual.fg_style = hss->diff_menu->selection;
            hss->focused_menu = DIFFS_MENU_ID;

            hss->redraw = 1;

            return &HIGHSCORES;
        }

        // NOTE since the nave menu only has one selection.
        // it doesn't really need to be updated here.

        return &HIGHSCORES;
    }

    // Here The difficulty menu must be in focus.

    // Move from difficulties to nav bar.
    if (hss->diff_menu->selection == DIFFS_LABELS_LEN - 1 && 
            (key_press(c_Down) || key_press(c_5))) {
        unfocus_basic_text_menu(hss->diff_menu);
        hss->scores_pane->slide.actual.bg_style = BLACK;
        hss->scores_pane->slide.actual.fg_style = SELECT_DIFF_FG;

        focus_basic_text_menu(hss->nav_menu);
        hss->focused_menu = NAV_MENU_ID;

        hss->redraw = 1;

        return &HIGHSCORES;
    }

    if (update_basic_text_menu(hss->diff_menu)) {
        hss->scores_pane->slide.actual.fg_style = 
            hss->diff_menu->selection;

        hss->redraw = 1;
    }

    return &HIGHSCORES;
}

static void render_highscores(void *glb_state, void *loc_state) {
    ms_scoreboard *sb = (ms_scoreboard *)glb_state;
    highscores_state *hss = (highscores_state *)loc_state;

    if (!hss->redraw) {
        return;
    }
    
    render_text_menu_nc(hss->diff_menu->super);
    render_text_menu_nc(hss->nav_menu->super);
    render_slide_pane_nc(hss->scores_pane, sb);

    gfx_SwapDraw();
    hss->redraw = 1;
}

static void *exit_highscores(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    highscores_state *hss = (highscores_state *)loc_state;

    del_slide_pane(hss->scores_pane);
    del_basic_text_menu(hss->diff_menu);
    del_basic_text_menu(hss->nav_menu);

    safe_free(HIGHSCORES_CHANNEL, hss);

    return NULL;
}

const loc_life_cycle HIGHSCORES = {
    .enter = enter_highscores,
    .update = update_highscores,
    .render = render_highscores,
    .exit = exit_highscores
};
