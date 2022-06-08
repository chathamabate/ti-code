#include "states.h"

#include <stdlib.h>
#include <graphx.h>
#include <cutil/menu.h>
#include <cutil/misc.h>
#include <tice.h>

#include "ms_styles.h"
#include "gfx/logo.h"

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
    text_menu menu;

    uint8_t redraw;
    uint8_t selection;
} homepage_state;

static void *enter_homepage(void *glb_state, void *trans_state) {
    (void)glb_state;

    // Transition state into the homepage should always be NULL.
    (void)trans_state;

    // Allocate the homepage local state.
    homepage_state *hp_state = safe_malloc(sizeof(homepage_state));

    hp_state->menu.template = &MENU_TEMPLATE;
    hp_state->menu.styles = safe_malloc(sizeof(uint8_t) * BTN_LABELS_LEN);

    hp_state->redraw = 1;
    hp_state->selection = 0;

    return hp_state;
}

static const loc_life_cycle *update_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    if (os_GetCSC() == sk_Clear) {
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
    gfx_SwapDraw();
}

static void *exit_homepage(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    // Extract home page state.
    homepage_state *hp_state = (homepage_state *)loc_state;

    // Free styles array.
    free(hp_state->menu.styles);

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