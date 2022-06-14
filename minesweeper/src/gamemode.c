#include "states.h"

#include <stdint.h>
#include <stdlib.h>
#include <tice.h>
#include <graphx.h>

#include <cutil/keys.h>
#include <cutil/menu.h>
#include <cutil/misc.h>

#include "ms_styles.h"
#include "ms_misc.h"
#include "minesweeper.h"

// Game mode code..

#define FOCUSED_KEYS_LEN 10

static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Up, c_Down, c_Left, c_Right,
    c_8, c_5, c_4, c_6,
    c_Enter, c_Clear
};

#define INFO_SPACER 4
#define DIFF_H_SCALE 2
#define DIFF_W_SCALE 1
#define INFO_H_SCALE 1
#define INFO_W_SCALE 1

#define INFO_HEIGHT (DIFF_H_SCALE + (2 * INFO_H_SCALE) + (2 * INFO_SPACER))

static void render_difficulty_info(const slide_pane_template *tmplt,
        const char *diff_name, const difficulty *diff) {
    uint16_t x_p;
    uint8_t y_p;

    gfx_SetTextScale(DIFF_W_SCALE, DIFF_H_SCALE);    
    x_p = tmplt->x + (tmplt->pane_width - gfx_GetStringWidth(diff_name)) / 2;
    y_p = tmplt->y + (tmplt->pane_height - INFO_HEIGHT) / 2;

    // Print difficulty name.
    gfx_PrintStringXY(diff_name, x_p, y_p);

    // Buffer for creating info strings.
    // 30 is by far big enough.
    char buff[30];

    gfx_SetTextScale(INFO_W_SCALE, INFO_H_SCALE);
}

static const slide_pane_template DIFF_PANE_TEMPLATE = {
    .x = align(1),
    .y = align(4),
    .pane_height = align(7),
    .pane_width = align(18),
    
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
};

#define GM_MENU_LABELS_LEN 3

static const char *GM_MENU_LABELS[GM_MENU_LABELS_LEN] = {
    "Easy", "Medium", "Hard"
};

#define MENU_BTN_WIDTH 96 
#define MENU_BTN_HEIGHT 32

static const text_menu_template GM_MENU_TEMPLATE = {
    .button_height = MENU_BTN_HEIGHT,
    .button_width = MENU_BTN_WIDTH,
    .format = MENU_HORIZONTAL,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = GM_MENU_LABELS,
    .len = GM_MENU_LABELS_LEN,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = (LCD_WIDTH - (GM_MENU_LABELS_LEN * MENU_BTN_WIDTH)) / 2,
    .y = 16
};

#define PLAY_MENU_LABELS_LEN 2

static const char *PLAY_MENU_LABELS[PLAY_MENU_LABELS_LEN] = {
    "Back", "Play"
};

static const text_menu_template PLAY_MENU_TEMPLATE = {
    .button_height = MENU_BTN_HEIGHT,
    .button_width = MENU_BTN_WIDTH, // Same width as GM_MENU.
    .format = MENU_HORIZONTAL,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = PLAY_MENU_LABELS,
    .len = PLAY_MENU_LABELS_LEN,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = (LCD_WIDTH - (PLAY_MENU_LABELS_LEN * MENU_BTN_WIDTH)) / 2,
    .y = LCD_HEIGHT - 16 - MENU_BTN_HEIGHT
};

#define GM_MENU 0
#define PLAY_MENU 1

typedef struct {
    // Display difficulties in this pane.
    slide_pane *diff_pane;

    toggle_text_menu *gm_menu;
    basic_text_menu *play_menu;

    uint8_t focused_menu;
    uint8_t redraw;
} gamemode_state;

// Template Local Life Cycle...

static void *enter_gamemode(void *glb_state, void *trans_state) {
    // Global state not used here.
    (void)glb_state;

    // Gamemode page gets a NULL transiiton state.
    (void)trans_state;

    gamemode_state *gm_state = safe_malloc(sizeof(gamemode_state));

    gm_state->gm_menu = new_toggle_text_menu(&GM_MENU_TEMPLATE, &MS_MENU_SS);
    focus_toggle_text_menu(gm_state->gm_menu);
    gm_state->focused_menu = GM_MENU;

    gm_state->play_menu = new_basic_text_menu(&PLAY_MENU_TEMPLATE, &MS_MENU_SS);

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    render_random_bg();
    // cgfx_pane_nc(&PANE_STYLE_0, align(1), align(4), align(18), align(7));
    gfx_BlitBuffer();

    gm_state->redraw = 1;

    return gm_state;
}

static const loc_life_cycle *update_gamemode(void *glb_state, void *loc_state) {
    (void)glb_state;
    
    gamemode_state *gm_state = (gamemode_state *)loc_state;

    scan_focused_keys();

    // Back button functionality.
    if (key_press(c_Enter) && gm_state->focused_menu == PLAY_MENU && gm_state->play_menu->selection == 0) {
        return &HOMEPAGE;
    }

    if ((key_press(c_Down) || key_press(c_5)) && gm_state->focused_menu == GM_MENU) {
        unfocus_toggle_text_menu(gm_state->gm_menu);

        focus_basic_text_menu(gm_state->play_menu);
        gm_state->focused_menu = PLAY_MENU;

        gm_state->redraw = 1;

        return &GAMEMODE;
    }

    if ((key_press(c_Up) || key_press(c_8)) && gm_state->focused_menu == PLAY_MENU) {
        unfocus_basic_text_menu(gm_state->play_menu);

        focus_toggle_text_menu(gm_state->gm_menu);
        gm_state->focused_menu = GM_MENU;

        gm_state->redraw = 1;

        return &GAMEMODE;
    }

    gm_state->redraw |= gm_state->focused_menu == PLAY_MENU 
        ? update_basic_text_menu(gm_state->play_menu)
        : update_toggle_text_menu(gm_state->gm_menu);

    return &GAMEMODE;
}

static void render_gamemode(void *glb_state, void *loc_state) {
    (void)glb_state;
    
    gamemode_state *gm_state = (gamemode_state *)loc_state;

    if (!gm_state->redraw) {
        return;
    }
    
    render_text_menu_nc(gm_state->gm_menu->super);
    render_text_menu_nc(gm_state->play_menu->super);
    gfx_SwapDraw();

    gm_state->redraw = 0;
}

static void *exit_gamemode(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    gamemode_state *gm_state = (gamemode_state *)loc_state;

    del_toggle_text_menu(gm_state->gm_menu);
    del_basic_text_menu(gm_state->play_menu);
    free(gm_state);

    return NULL;
}

const loc_life_cycle GAMEMODE = {
    .enter = enter_gamemode,
    .update = update_gamemode,
    .render = render_gamemode,
    .exit = exit_gamemode
};
