#include "menu.h"

#include <tice.h>
// #include <keypadc.h>
#include <graphx.h>

#include <cutil/keys.h>
#include <cutil/misc.h>
#include <cutil/cgraphx.h>

// Create a new text menu.
text_menu *new_text_menu(const text_menu_template *tmplt, uint8_t style) {
    text_menu *menu = safe_malloc(MENU_CHANNEL, sizeof(text_menu));

    menu->tmplt = tmplt;
    menu->styles = (buffered_styling *)safe_malloc(MENU_CHANNEL, sizeof(buffered_styling) * tmplt->len);

    uint8_t i;
    for (i = 0; i < tmplt->len; i++) {
        menu->styles[i].buffer_style = MENU_BTN_NOT_RENDERED;
        menu->styles[i].screen_style = MENU_BTN_NOT_RENDERED;
        menu->styles[i].style = style;
    }

    return menu;
}

// Render a single button of a menu.
// Always performs redraw, does not rotate styling states.
static void render_text_menu_button_nc(text_menu *menu, uint8_t i) {
    const text_menu_template *template = menu->tmplt;
    const cgfx_pane_style *pane_style = template->style_palette[menu->styles[i].style];

    uint16_t x_p;
    uint8_t y_p; 

    if (template->format == MENU_VERTICAL) {
        x_p = template->x;
        y_p = template->y + (i * template->button_height);
    } else {
        x_p = template->x + (i * template->button_width);
        y_p = template->y;
    }

    cgfx_pane_nc(pane_style, x_p, y_p, template->button_width, template->button_height);

    gfx_SetTextScale(template->label_width_scale, template->label_height_scale);
    gfx_SetTextFGColor(pane_style->text_fg);

    uint16_t label_width = gfx_GetStringWidth(template->labels[i]);

    gfx_PrintStringXY(
        template->labels[i], 
        x_p + ((template->button_width - label_width) / 2), 
        y_p + ((template->button_height - template->label_height_scale*8) / 2)
    );
}

void render_text_menu_nc(text_menu *menu) {
    buffered_styling *styles = menu->styles;

    uint8_t i, len = menu->tmplt->len;
    for (i = 0; i < len; i++) {
        // Only render a button if it needs to be rendered.
        if (styles[i].buffer_style != styles[i].style) {
            render_text_menu_button_nc(menu, i);
        }

        // Regardless of whether drawing occured, rotate the styles.
        styles[i].buffer_style = styles[i].screen_style;
        styles[i].screen_style = styles[i].style;
    }
}

void reset_render_text_menu(text_menu *menu) {
    uint8_t i;
    for (i = 0; i < menu->tmplt->len; i++) {
        menu->styles[i].buffer_style = MENU_BTN_NOT_RENDERED;
        menu->styles[i].screen_style = MENU_BTN_NOT_RENDERED;
    }
}

void del_text_menu(text_menu *menu) {
    safe_free(MENU_CHANNEL, menu->styles);
    safe_free(MENU_CHANNEL, menu);
}

basic_text_menu *new_basic_text_menu(const text_menu_template *tmplt, const selection_styling *ss) {
    // Start unfocused.
    text_menu *super = new_text_menu(tmplt, ss->unfocus_style);

    basic_text_menu *tt_menu = safe_malloc(MENU_CHANNEL, sizeof(basic_text_menu));
    tt_menu->super = super;
    tt_menu->ss = ss;

    // Defualt to first option selected.
    tt_menu->selection = 0;

    return tt_menu;
}

void focus_basic_text_menu(basic_text_menu *tt_menu) {
    uint8_t i, len = tt_menu->super->tmplt->len;
    for (i = 0; i < len; i++) {
        tt_menu->super->styles[i].style = tt_menu->ss->deselection_style;
    }

    tt_menu->super->styles[tt_menu->selection].style = tt_menu->ss->selection_style;
}

void unfocus_basic_text_menu(basic_text_menu *tt_menu) {
    uint8_t i, len = tt_menu->super->tmplt->len;
    for (i = 0; i < len; i++) {
        tt_menu->super->styles[i].style = tt_menu->ss->unfocus_style;
    }
}

// If a key signaling previous was hit.
#define advance_previous(format) \
    (((format) == MENU_VERTICAL && (key_press(c_Up) || key_press(c_8))) || \
    ((format) == MENU_HORIZONTAL && (key_press(c_Left) || key_press(c_4))))

// If a key signaling next was hit.
#define advance_next(format) \
    (((format) == MENU_VERTICAL && (key_press(c_Down) || key_press(c_5))) || \
    ((format) == MENU_HORIZONTAL && (key_press(c_Right) || key_press(c_6)))) 

uint8_t update_basic_text_menu(basic_text_menu *tt_menu) {
    uint8_t format = tt_menu->super->tmplt->format;
    buffered_styling *styles = tt_menu->super->styles;

    if (advance_previous(format) && tt_menu->selection != 0) {
        styles[tt_menu->selection].style = tt_menu->ss->deselection_style;
        styles[--(tt_menu->selection)].style = tt_menu->ss->selection_style;

        return 1;
    }
    
    if (advance_next(format) && tt_menu->selection != tt_menu->super->tmplt->len - 1) {
        styles[tt_menu->selection].style = tt_menu->ss->deselection_style;
        styles[++(tt_menu->selection)].style = tt_menu->ss->selection_style;

        return 1;
    }

    return 0;
}

void del_basic_text_menu(basic_text_menu *bt_menu) {
    del_text_menu(bt_menu->super);
    safe_free(MENU_CHANNEL, bt_menu);
}

// NOTE : 
// This is a little confusing...
// But all toggle menus will follow the following stylings...
//
// When Focused...
// toggle : selection_style
// selection : deselection_style
// other : unfocus_style
//
// (If toggle = selection, then selection_style)
//
// When Unfocused...
// toggle : selection_style
// other : unfocus_style

toggle_text_menu *new_toggle_text_menu(const text_menu_template *tmplt, const selection_styling *ss) {
    // Start unfocused.
    text_menu *super = new_text_menu(tmplt, ss->unfocus_style);

    toggle_text_menu *tt_menu = safe_malloc(MENU_CHANNEL, sizeof(toggle_text_menu));
    tt_menu->super = super;
    tt_menu->ss = ss;

    // Arbitrary defualt selection and toggle.
    tt_menu->selection = 0;
    tt_menu->toggle = tmplt->len - 1;

    // Selection will be style of the unfocused toggle.
    super->styles[tmplt->len - 1].style = ss->selection_style;

    return tt_menu;
}

void focus_toggle_text_menu(toggle_text_menu *tt_menu) {
    uint8_t i, len = tt_menu->super->tmplt->len;
    for (i = 0; i < len; i++) {
        tt_menu->super->styles[i].style = tt_menu->ss->unfocus_style;
    }
    
    tt_menu->super->styles[tt_menu->selection].style = tt_menu->ss->deselection_style;
    tt_menu->super->styles[tt_menu->toggle].style = tt_menu->ss->selection_style;
}

void unfocus_toggle_text_menu(toggle_text_menu *tt_menu) {
    uint8_t i, len = tt_menu->super->tmplt->len;
    for (i = 0; i < len; i++) {
        tt_menu->super->styles[i].style = tt_menu->ss->unfocus_style;
    }

    tt_menu->super->styles[tt_menu->toggle].style = tt_menu->ss->selection_style;
}

uint8_t update_toggle_text_menu(toggle_text_menu *tt_menu) {
    uint8_t format = tt_menu->super->tmplt->format;
    buffered_styling *styles = tt_menu->super->styles;
    const selection_styling *ss = tt_menu->ss;

    if (key_press(c_Enter)) {
        if (tt_menu->selection == tt_menu->toggle) {
            return 0;
        }
        
        // Toggle is being changed!
        styles[tt_menu->toggle].style = ss->unfocus_style;

        tt_menu->toggle = tt_menu->selection;
        styles[tt_menu->toggle].style = ss->selection_style;

        return 1;
    }

    if (advance_previous(format) && tt_menu->selection != 0) {
        styles[tt_menu->selection].style = tt_menu->selection == tt_menu->toggle 
            ? ss->selection_style : ss->unfocus_style;

        tt_menu->selection--;

        styles[tt_menu->selection].style = tt_menu->selection == tt_menu->toggle
            ? ss->selection_style : ss->deselection_style;

        return 1;
    }
    
    if (advance_next(format) && tt_menu->selection != tt_menu->super->tmplt->len - 1) {
        styles[tt_menu->selection].style = tt_menu->selection == tt_menu->toggle 
            ? tt_menu->ss->selection_style : tt_menu->ss->unfocus_style;

        tt_menu->selection++;

        styles[tt_menu->selection].style = tt_menu->selection == tt_menu->toggle 
            ? ss->selection_style : ss->deselection_style;

        return 1;
    }

    return 0;
}

void del_toggle_text_menu(toggle_text_menu *tt_menu) {
    del_text_menu(tt_menu->super);
    safe_free(MENU_CHANNEL, tt_menu);
}

// Slide Pane code.

const render UNRENDERED_SLIDE = {
    .bg_style = MENU_BTN_NOT_RENDERED,
    .fg_style = MENU_BTN_NOT_RENDERED
};

slide_pane *new_slide_pane(const slide_pane_template *tmp, render init_render) {
    slide_pane *s_pane = safe_malloc(MENU_CHANNEL, sizeof(slide_pane)); 
    s_pane->tmplt = tmp;

    s_pane->slide.buffer_render = UNRENDERED_SLIDE;
    s_pane->slide.screen_render = UNRENDERED_SLIDE;

    // To be rendered first time around.
    s_pane->slide.actual = init_render;

    return s_pane;
}

void render_slide_pane_nc(slide_pane *s_pane, void *data) {
    const slide_pane_template *tmplt = s_pane->tmplt;

    if (!equ_render(s_pane->slide.actual, s_pane->slide.buffer_render)) {
        // In this case a re render is needed.

        // Render background.
        cgfx_pane_nc(
                tmplt->style_palette[s_pane->slide.actual.bg_style],
                tmplt->x, tmplt->y,
                tmplt->pane_width, tmplt->pane_height
        );

        // Render foreground.
        (tmplt->slide_renderers[s_pane->slide.actual.fg_style])(tmplt, data);
    }

    // No matter what, rotate buffer and screen renders.
    s_pane->slide.buffer_render = s_pane->slide.screen_render;
    s_pane->slide.screen_render = s_pane->slide.actual;
}
