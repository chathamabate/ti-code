#include "menu.h"

#include <tice.h>
#include <graphx.h>

#include <cutil/misc.h>
#include <cutil/cgraphx.h>

void render_text_menu_button_nc(text_menu *menu, uint16_t x, uint8_t y, uint8_t i) {
    const text_menu_template *template = menu->template;
    const cgfx_pane_style *pane_style = template->style_palette[menu->styles[i]];

    uint16_t x_p;
    uint8_t y_p; 

    if (template->format == MENU_VERTICAL) {
        x_p = x;
        y_p = y + (i * template->button_height);
    } else {
        x_p = x + (i * template->button_width);
        y_p = y;
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

void render_text_menu_nc(text_menu *menu, uint16_t x, uint8_t y) {
    uint8_t i;
    for (i = 0; i < menu->template->len; i++) {
        render_text_menu_button_nc(menu, x, y, i);
    }
}

basic_text_menu *new_basic_text_menu(const text_menu_template *tmplt, uint8_t s_style, uint8_t ds_style) {
    basic_text_menu *bt_menu = safe_malloc(sizeof(basic_text_menu));

    bt_menu->menu.template = tmplt;
    bt_menu->menu.styles = safe_malloc(sizeof(uint8_t) * tmplt->len);

    bt_menu->menu.styles[0] = s_style;

    uint8_t i;
    for (i = 1; i < tmplt->len; i++) {
        bt_menu->menu.styles[i] = ds_style;
    }

    bt_menu->selection_style = s_style;
    bt_menu->deselection_style = ds_style;

    bt_menu->selection = 0;

    return bt_menu;
}

uint8_t update_basic_text_menu(basic_text_menu *bt_menu) {
    // Consider switching to is down.
    sk_key_t key = 0; 
    uint8_t format = bt_menu->menu.template->format;

    if (
        ((format == MENU_VERTICAL && (key == sk_Up || key == sk_8)) ||
        (format == MENU_HORIZONTAL && (key == sk_Left || key == sk_4))) &&
        bt_menu->selection != 0 
    ) {
        bt_menu->menu.styles[bt_menu->selection] = bt_menu->deselection_style;
        bt_menu->menu.styles[--(bt_menu->selection)] = bt_menu->selection_style;

        return 1;
    } else if (
        ((format == MENU_VERTICAL && (key == sk_Down || key == sk_5)) ||
        (format == MENU_HORIZONTAL && (key == sk_Right || key == sk_6))) &&
        bt_menu->selection != bt_menu->menu.template->len - 1
    ) {
        bt_menu->menu.styles[bt_menu->selection] = bt_menu->deselection_style;
        bt_menu->menu.styles[++(bt_menu->selection)] = bt_menu->selection_style;

        return 1;
    }

    return 0;
}

void del_basic_text_menu(basic_text_menu *bt_menu) {
    free(bt_menu->menu.styles);
    free(bt_menu);
}

