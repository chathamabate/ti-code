#include "states.h"

#include <stdlib.h>
#include <graphx.h>
#include <cutil/menu.h>
#include <cutil/misc.h>
#include <cutil/keys.h>
#include <tice.h>
#include <keypadc.h>

#include "ms_styles.h"
#include "ms_misc.h"
#include "ms_mem_channels.h"
#include "gfx/logo.h"

#define FOCUSED_KEYS_LEN 5

static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Up, c_Down, 
    c_8, c_5,
    c_Enter
};

#define BTN_LABELS_LEN 4

// Main menu button labels.
static const char *BTN_LABELS[BTN_LABELS_LEN] = {
    "Play",
    "Highscores",
    "Instructions",
    "Exit"
};

#define BTN_WIDTH 128

static const text_menu_template MENU_TEMPLATE = {
    .button_height = 24,
    .button_width = BTN_WIDTH,
    .format = MENU_VERTICAL,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = BTN_LABELS,
    .len = BTN_LABELS_LEN,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = (LCD_WIDTH - BTN_WIDTH) / 2,
    .y = 128
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
    homepage_state *hp_state = safe_malloc(HOMEPAGE_CHANNEL, sizeof(homepage_state));

    hp_state->bt_menu = new_basic_text_menu(&MENU_TEMPLATE, &MS_MENU_SS);

    // Only menu on the screen... focus it from the get go.
    focus_basic_text_menu(hp_state->bt_menu);

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    // Perform background Render.
    render_random_bg();
    cgfx_pane_nc(&PANE_STYLE_0, (320 - 224) / 2, 16, 224, 80);
    gfx_RLETSprite_NoClip(logo, (LCD_WIDTH - logo_width) / 2, 16 + (80 - 64) / 2);
    gfx_BlitBuffer(); // Copy background to the screen.
    
    // Signal a menu redraw.
    hp_state->redraw = 1;

    return hp_state;
}

static const loc_life_cycle *update_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;

    homepage_state *hp_state = (homepage_state *)loc_state;

    // This includes a kb_Scan().
    scan_focused_keys();

    if (key_press(c_Enter)) {
        switch (hp_state->bt_menu->selection) {
        case 0: // Play
            return &GAMEMODE;
        case 1: // Highscores
            break;
        case 2: // Instructions
            break;
        case 3: // Exit
            return NULL;
        }
    }

    hp_state->redraw |= update_basic_text_menu(hp_state->bt_menu);

    return &HOMEPAGE;
}

static void render_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;

    homepage_state *hp_state = (homepage_state *)loc_state;

    if (!hp_state->redraw) {
        return;
    }

    // Only render the menu.
    render_text_menu_nc(hp_state->bt_menu->super);
    gfx_SwapDraw();

    hp_state->redraw = 0;
}

static void *exit_homepage(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    // Extract home page state.
    homepage_state *hp_state = (homepage_state *)loc_state;

    del_basic_text_menu(hp_state->bt_menu);

    // Free the state as a whole.
    safe_free(HOMEPAGE_CHANNEL, hp_state);

    return NULL;
}

const loc_life_cycle HOMEPAGE = {
    .enter = enter_homepage,
    .update = update_homepage,
    .render = render_homepage,
    .exit = exit_homepage
};
