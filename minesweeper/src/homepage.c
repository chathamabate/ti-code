#include "states.h"

#include <stdlib.h>
#include <graphx.h>
#include <cutil/menu.h>
#include <cutil/misc.h>
#include <cutil/keys.h>
#include <tice.h>
#include <keypadc.h>

#include "ms_styles.h"
#include "gfx/logo.h"

#define FOCUSED_KEYS_LEN 10

static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Up, c_Down, c_Left, c_Right, 
    c_8, c_5, c_4, c_6,
    c_Enter, c_Clear
};

#define BTN_LABELS_LEN 4

// Main menu button labels.
static const char *BTN_LABELS[BTN_LABELS_LEN] = {
    "Play",
    "Highscores",
    "Instructions",
    "Exit"
};

static const text_menu_template MENU_TEMPLATE = {
    .button_height = 24,
    .button_width = 128,
    .format = MENU_VERTICAL,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = BTN_LABELS,
    .len = BTN_LABELS_LEN,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN
};

// The local state for the homepage.
typedef struct {
    basic_text_menu *bt_menu;

    uint8_t redraw;
} homepage_state;

static void *enter_homepage(void *glb_state, void *trans_state) {
    (void)glb_state;

    // Transition state into the homepage should always be NULL.
    (void)trans_state;

    // Allocate the homepage local state.
    homepage_state *hp_state = safe_malloc(sizeof(homepage_state));

    hp_state->bt_menu = new_basic_text_menu(&MENU_TEMPLATE, 1, 0);

    hp_state->redraw = 1;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return hp_state;
}

static const loc_life_cycle *update_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;

    homepage_state *hp_state = (homepage_state *)loc_state;

    // This includes a kb_Scan().
    scan_focused_keys();

    hp_state->redraw |= update_basic_text_menu(hp_state->bt_menu);

    if (kb_IsDown(kb_KeyClear)) {
        return NULL; // Exit case.
    }

    return &HOMEPAGE;
}

static void render_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;

    homepage_state *hp_state = (homepage_state *)loc_state;

    if (!hp_state->redraw) {
        return;
    }

    gfx_FillScreen(0x06);
    gfx_RLETSprite_NoClip(logo, (LCD_WIDTH - logo_width) / 2, 16);
    render_text_menu_nc(&hp_state->bt_menu->menu, 
        (LCD_WIDTH - hp_state->bt_menu->menu.template->button_width) / 2, 128);

    gfx_SwapDraw();
}

static void *exit_homepage(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    // Extract home page state.
    homepage_state *hp_state = (homepage_state *)loc_state;

    del_basic_text_menu(hp_state->bt_menu);

    // Free the state as a whole.
    free(hp_state);

    return NULL;
}

const loc_life_cycle HOMEPAGE = {
    .enter = enter_homepage,
    .update = update_homepage,
    .render = render_homepage,
    .exit = exit_homepage
};