
#include "cutil/misc.h"
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

typedef struct {
    basic_text_menu *nav;
    uint8_t diff_ind;
    uint8_t redraw;
} defeat_state;

static const char *DEFEAT_MSG = "Defeat!";

static void *enter_defeat(void *glb_state, void *trans_state) {
    (void)glb_state;

    uint8_t *diff_ptr = (uint8_t *)trans_state;
    
    defeat_state *ds = safe_malloc(DEFEAT_CHANNEL, sizeof(defeat_state));
    ds->diff_ind = *diff_ptr;
    safe_free(DEFEAT_CHANNEL, diff_ptr);

    ds->nav = new_basic_text_menu(&NAV_TMPLT, &MS_MENU_SS);

    render_random_bg();

    // Taken from victory.c
    cgfx_pane_nc(&PANE_STYLE_3, align(5), align(4), align(10), align(4));
    gfx_SetTextScale(2, 4);
    gfx_SetTextFGColor(1);
    gfx_PrintStringXY(DEFEAT_MSG, (LCD_WIDTH - gfx_GetStringWidth(DEFEAT_MSG)) / 2, align(5));

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
        return &HOMEPAGE;
    }

    ds->redraw = update_basic_text_menu(ds->nav);

    return &DEFEAT;
}

static void render_defeat(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_defeat(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    defeat_state *ds = (defeat_state *)loc_state;

    del_basic_text_menu(ds->nav);
    safe_free(DEFEAT_CHANNEL, ds);

    return NULL;
}

const loc_life_cycle DEFEAT = {
    .enter = enter_defeat,
    .update = update_defeat,
    .render = render_defeat,
    .exit = exit_defeat
};
