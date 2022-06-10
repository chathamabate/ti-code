#include "menu.h"

#include <tice.h>
// #include <keypadc.h>
#include <graphx.h>

#include <cutil/keys.h>
#include <cutil/misc.h>
#include <cutil/cgraphx.h>

// Render a single button of a menu.
void render_text_menu_button_nc(text_menu *menu, uint8_t i) {
    const text_menu_template *template = menu->template;
    const cgfx_pane_style *pane_style = template->style_palette[menu->styles[i]];

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

    uint16_t label_width = gfx_GetStringWidth(template->labels[i]);

    gfx_SetTextScale(template->label_width_scale, template->label_height_scale);
    gfx_SetTextFGColor(pane_style->text_fg);

    gfx_PrintStringXY(
        template->labels[i], 
        x_p + ((template->button_width - label_width) / 2), 
        y_p + ((template->button_height - template->label_height_scale*8) / 2)
    );
}

void render_text_menu_nc(text_menu *menu) {
    uint8_t i;
    for (i = 0; i < menu->template->len; i++) {
        render_text_menu_button_nc(menu, i);
    }
}

simple_text_menu *new_simple_text_menu(const text_menu_template *tmplt, 
    uint8_t s_style, uint8_t ds_style, uint8_t uf_style) {
    
    simple_text_menu *st_menu = safe_malloc(sizeof(simple_text_menu));

    st_menu->menu.template = tmplt;
    st_menu->menu.styles = (uint8_t *)safe_malloc(sizeof(uint8_t) * tmplt->len);

    // Menu starts off as unfocused.
    uint8_t i;
    for (i = 0; i < tmplt->len; i++) {
        st_menu->menu.styles[i] = uf_style;
    }

    // Store selection styles.
    st_menu->selection_style = s_style;
    st_menu->deselection_style = ds_style;
    st_menu->unfocused_style = uf_style;

    // Default all selections to 0.
    st_menu->buffer_selection = 0;
    st_menu->screen_selection = 0;
    st_menu->selection = 0;

    st_menu->redraw = FULL_REDRAW;

    return st_menu;
}

void focus_simple_text_menu(simple_text_menu *st_menu) {
    // TODO...
}

void unfocus_simple_text_menu(simple_text_menu *st_menu) {
    uint8_t len = st_menu->menu.template->len;

    uint8_t i;
    for (i = 0; i < len; i++) {
        st_menu->menu.styles[i] = st_menu->unfocused_style;
    }

    st_menu->redraw = FULL_REDRAW;
}

void update_simple_text_menu(simple_text_menu *st_menu) {
    uint8_t format = st_menu->menu.template->format;

    if (
        ((format == MENU_VERTICAL && (key_press(c_Up) || key_press(c_8))) ||
        (format == MENU_HORIZONTAL && (key_press(c_Left) || key_press(c_4)))) &&
        st_menu->selection != 0 
    ) {
        st_menu->menu.styles[st_menu->selection] = st_menu->deselection_style;
        st_menu->menu.styles[--(st_menu->selection)] = st_menu->selection_style;
    } else if (
        ((format == MENU_VERTICAL && (key_press(c_Down) || key_press(c_5))) ||
        (format == MENU_HORIZONTAL && (key_press(c_Right) || key_press(c_6)))) &&
        st_menu->selection != st_menu->menu.template->len - 1
    ) {
        st_menu->menu.styles[st_menu->selection] = st_menu->deselection_style;
        st_menu->menu.styles[++(st_menu->selection)] = st_menu->selection_style;
    }

    // Specify to renderer if redraw must occur.
    st_menu->redraw = st_menu->selection == st_menu->buffer_selection 
        ? NO_REDRAW 
        : PARTIAL_REDRAW;
}

uint8_t render_simple_text_menu_nc(simple_text_menu *st_menu) {
    if (st_menu->redraw == FULL_REDRAW) {
        // Full menu redraw required.
        render_text_menu_nc(&(st_menu->menu));

        goto ROTATE_SELECTIONS;
    }

    if (st_menu->redraw == PARTIAL_REDRAW) {
        // This signifies the selection has changed!

        // Now we are drawing over the buffer.
        render_text_menu_button_nc(&(st_menu->menu), st_menu->buffer_selection);
        render_text_menu_button_nc(&(st_menu->menu), st_menu->selection);

        goto ROTATE_SELECTIONS;
    }

    return 0;

ROTATE_SELECTIONS:
    st_menu->buffer_selection = st_menu->screen_selection;
    st_menu->screen_selection = st_menu->selection;

    return 1;
}

void del_simple_text_menu(simple_text_menu *st_menu) {
    free(st_menu->menu.styles);
    free(st_menu);
}

