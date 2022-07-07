#include <cutil/gameloop.h>
#include "graphx.h"
#include "states.h"
#include <cutil/keys.h>
#include <cutil/menu.h>
#include <cutil/misc.h>
#include <cutil/cgraphx.h>
#include <stdio.h>
#include "ms_mem_channels.h"
#include "ms_styles.h"
#include "ms_misc.h"

#define FOCUSED_KEYS_LEN 5
static const c_key_t FOCUSED_KEYS[FOCUSED_KEYS_LEN] = {
    c_Left, c_Right,
    c_4, c_6,
    c_Enter
};

static const text_pane_template TITLE_TMPLT = {
    .x = align(6),
    .y = align(1),
    .w = align(8),
    .h = align(3),
    .style = &PANE_STYLE_0,
    .text = "Instructions",
    .text_color = 1,
    .text_h_sc = 2,
    .text_w_sc = 1
};

#define NAV_LABELS_LEN 3
static const char *NAV_LABELS[NAV_LABELS_LEN] = {
    "<", "Back", ">"
};

static const text_menu_template NAV_TMPLT = {
    .button_height = align(2),
    .button_width = align(4),
    .label_height_scale = 1,
    .label_width_scale = 1,
    .labels = NAV_LABELS,
    .len = NAV_LABELS_LEN,
    .format = MENU_HORIZONTAL,
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN,
    .x = align(4),
    .y = align(12)
};

// Instruction pane.
typedef struct {
    const char *title;
    const char **msgs;
    uint8_t msgs_len;
} i_pane;

#define CNTRLS_PANE_MSGS_LEN 4
static const char *CNTRLS_PANE_MSGS[CNTRLS_PANE_MSGS_LEN] = {
    "Move using arrows or 8, 4, 5, and 6.",
    "Press 7 to flag a cell.",
    "Press 9 or Enter to uncover a cell.",
    "Press Clear to pause."
};

static const i_pane CNTRLS_PANE = {
    .title = "Controls",
    .msgs = CNTRLS_PANE_MSGS,
    .msgs_len = CNTRLS_PANE_MSGS_LEN
};

#define GP_PANE_MSGS_LEN 4
static const char *GP_PANE_MSGS[GP_PANE_MSGS_LEN] = {
    "A cell's number is equal to how",
    "many mines surround the cell.",
    "Uncovering a mine results in",
    "a loss!"
};

static const i_pane GP_PANE = {
    .title = "Gameplay",
    .msgs = GP_PANE_MSGS,
    .msgs_len = GP_PANE_MSGS_LEN
};


#define OBJ_PANE_MSGS_LEN 2
static const char *OBJ_PANE_MSGS[OBJ_PANE_MSGS_LEN] = {
    "Uncover all non-mine cells!",
    "Do so as fast as possible."
};

static const i_pane OBJ_PANE = {
    .title = "Objective",
    .msgs = OBJ_PANE_MSGS,
    .msgs_len = OBJ_PANE_MSGS_LEN
};

// Objective.
// Controls.
// Gameplay.

#define LINE_HEIGHT 12
#define BORDER      8

#define TITLE_SPACE 4

static void render_i_pane(const slide_pane_template *tmplt, 
        const i_pane *ip, uint8_t pane, uint8_t panes) {
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(1);
    
    // Center title... than lines...
    uint16_t xp = slide_pane_center_text(tmplt, ip->title); 
    uint8_t yp = tmplt->y + BORDER;

    gfx_PrintStringXY(ip->title, xp, yp);

    yp += LINE_HEIGHT + TITLE_SPACE;

    // Print messages. (All Centered)
    uint8_t i;
    for (i = 0; i < ip->msgs_len; i++, yp += LINE_HEIGHT) {
        xp = slide_pane_center_text(tmplt, ip->msgs[i]);
        gfx_PrintStringXY(ip->msgs[i], xp, yp);
    }

    // Finally print the footer.
    // NOTE this assumes pg and pgs are both single digit numbers.
    char buff[4];
    sprintf(buff, "%d/%d", pane, panes);

    // Center footer as well.
    xp = slide_pane_center_text(tmplt, buff);
    yp = tmplt->y + tmplt->pane_height -
        (BORDER + 8);

    gfx_PrintStringXY(buff, xp, yp);
}

#define I_RENDERERS_LEN 3

static void render_controls(const slide_pane_template *tmplt, void *data) {
    (void)data;

    render_i_pane(tmplt, &CNTRLS_PANE, 1, I_RENDERERS_LEN);
}

static void render_gp(const slide_pane_template *tmplt, void *data) {
    (void)data;

    render_i_pane(tmplt, &GP_PANE, 2, I_RENDERERS_LEN);
}

static void render_obj(const slide_pane_template *tmplt, void *data) {
    (void)data;

    render_i_pane(tmplt, &OBJ_PANE, 3, I_RENDERERS_LEN);
}

static const slide_renderer I_RENDERERS[I_RENDERERS_LEN] = {
    render_controls,
    render_gp,
    render_obj
};

static const slide_pane_template I_TMPLT = {
    .pane_width = align(18),
    .pane_height = align(6),
    .slide_renderers = I_RENDERERS,
    .len = I_RENDERERS_LEN,
    .x = align(1),
    .y = align(5),
    .style_palette = PANE_STYLE_PALETTE,
    .style_palette_len = PANE_STYLE_PALETTE_LEN
};

typedef struct {
    basic_text_menu *nav;
    slide_pane *i_panes;
    uint8_t redraw;
} instructions_state;

static void *enter_instructions(void *glb_state, void *trans_state) {
    (void)glb_state;
    (void)trans_state;

    instructions_state *is = 
        safe_malloc(INSTRUCTIONS_CHANNEL, sizeof(instructions_state));

    is->nav = new_basic_text_menu(&NAV_TMPLT, &MS_MENU_SS);
    focus_basic_text_menu(is->nav);

    const render r = {
        .bg_style = 0,
        .fg_style = 0
    };

    is->i_panes = new_slide_pane(&I_TMPLT, r);

    render_random_bg();
    cgfx_text_pane_nc(&TITLE_TMPLT);
    gfx_BlitBuffer();

    is->redraw = 1;

    set_focused_keys(FOCUSED_KEYS, FOCUSED_KEYS_LEN);

    return is;
}

static const loc_life_cycle *update_instructions(void *glb_state, void *loc_state) {
    (void)glb_state;

    instructions_state *is = (instructions_state *)loc_state;

    scan_focused_keys();

    if (key_press(c_Enter)) {
        switch (is->nav->selection) { 
        case 0:
            if (is->i_panes->slide.actual.fg_style > 0) {
               is->i_panes->slide.actual.fg_style--; 
               is->redraw = 1;
            }

            return &INSTRUCTIONS;
        case 1:
            return &HOMEPAGE;
        case 2: 
            if (is->i_panes->slide.actual.fg_style < is->i_panes->tmplt->len - 1) {
               is->i_panes->slide.actual.fg_style++; 
               is->redraw = 1;
            }

            return &INSTRUCTIONS;
        }

    }

    is->redraw |= update_basic_text_menu(is->nav);

    return &INSTRUCTIONS;
}

static void render_instructions(void *glb_state, void *loc_state) {
    (void)glb_state;

    instructions_state *is = (instructions_state *)loc_state;

    if (!is->redraw) {
        return;
    }

    render_slide_pane_nc(is->i_panes, NULL);
    render_text_menu_nc(is->nav->super);
    gfx_SwapDraw();

    is->redraw = 0;
}

static void *exit_instructions(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)next_loc_lc;

    instructions_state *is = (instructions_state *)loc_state;

    del_basic_text_menu(is->nav);
    del_slide_pane(is->i_panes);

    safe_free(INSTRUCTIONS_CHANNEL, is);

    // No transition state needed for going back to main
    // menu.
    return NULL;
}

const loc_life_cycle INSTRUCTIONS = {
    .enter = enter_instructions,
    .update = update_instructions,
    .render = render_instructions,
    .exit = exit_instructions
};
