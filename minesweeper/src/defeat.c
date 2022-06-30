
#include "cutil/cgraphx.h"
#include "cutil/misc.h"
#include "graphx.h"
#include "states.h"

#include <cutil/menu.h>
#include <cutil/keys.h>
#include "ms_misc.h"
#include "ms_styles.h"
#include "ms_mem_channels.h"

#define FOCUSED_KEYS_LEN 5
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Up, c_Down, 
    c_8, c_5,
    c_Enter
};

#define LABELS_LEN 2
static const char *LABELS[LABELS_LEN] = {
    "Retry",
    "Main Menu"
};

static const text_menu_template NAV_TMPLT = {
    .button_height = align(2),
    .button_width = align(8),
    .x = align_horizontal(8),
    .y = align(10),
    .labels = LABELS,
    .len = LABELS_LEN,
    .label_height_scale = 1,
    .label_width_scale = 1,
    .format = MENU_VERTICAL,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN
};

static const text_pane_template TITLE_TMPLT = {
    .x = align(5),
    .y = align(4),
    .w = align(10),
    .h = align(4),

    .text_w_sc = 2,
    .text_h_sc = 4,
    .style = &PANE_STYLE_3,
    .text_color = 1,

    .text = "Defeat!"
};

typedef struct {
    basic_text_menu *nav;
    uint8_t diff_ind;
    uint8_t redraw;
} defeat_state;

static void *enter_defeat(void *glb_state, void *trans_state) {
    (void)glb_state;

    uint8_t *diff_ptr = (uint8_t *)trans_state;
    
    defeat_state *ds = safe_malloc(DEFEAT_CHANNEL, sizeof(defeat_state));
    ds->diff_ind = *diff_ptr;
    safe_free(DEFEAT_CHANNEL, diff_ptr);

    ds->nav = new_basic_text_menu(&NAV_TMPLT, &MS_MENU_SS);
    focus_basic_text_menu(ds->nav);

    render_random_bg();

    // Taken from victory.c
    cgfx_text_pane_nc(&TITLE_TMPLT);

    gfx_Blit(gfx_buffer);

    ds->redraw = 1;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return ds;
}

static const loc_life_cycle *update_defeat(void *glb_state, void *loc_state) {
    (void)glb_state;

    defeat_state *ds = (defeat_state *)loc_state;

    scan_focused_keys();

    if (key_press(c_Enter)) {
        return ds->nav->selection == 0 
            ? &GAMEPLAY : &HOMEPAGE;
    }

    ds->redraw |= update_basic_text_menu(ds->nav);

    return &DEFEAT;
}

static void render_defeat(void *glb_state, void *loc_state) {
    (void)glb_state;

    defeat_state *ds = (defeat_state *)loc_state;

    if (!ds->redraw) {
        return;
    }

    render_text_menu_nc(ds->nav->super);
    gfx_SwapDraw();

    ds->redraw = 0;
}

static void *exit_defeat(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    defeat_state *ds = (defeat_state *)loc_state;

    uint8_t diff_ind = ds->diff_ind;

    del_basic_text_menu(ds->nav);
    safe_free(DEFEAT_CHANNEL, ds);

    if (next_loc_lc == &HOMEPAGE) {
        return NULL;
    }

    // Otherwise replay.
    uint8_t *diff_ptr = safe_malloc(GAMEPLAY_CHANNEL, sizeof(uint8_t));
    *diff_ptr = diff_ind;

    return diff_ptr;
}

const loc_life_cycle DEFEAT = {
    .enter = enter_defeat,
    .update = update_defeat,
    .render = render_defeat,
    .exit = exit_defeat
};
