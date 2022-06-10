#include "menu.h"

#include <tice.h>
// #include <keypadc.h>
#include <graphx.h>

#include <cutil/keys.h>
#include <cutil/misc.h>
#include <cutil/cgraphx.h>

// Create a new text menu.
text_menu *new_text_menu(const text_menu_template *tmplt, uint8_t style) {
    text_menu *menu = safe_malloc(sizeof(text_menu));

    menu->template = tmplt;
    menu->styles = (buffered_styling *)safe_malloc(sizeof(buffered_styling) * tmplt->len);

    uint8_t i;
    for (i = 0; i < tmplt->len; i++) {
        menu->styles[i].buffer_style = NOT_RENDERED;
        menu->styles[i].screen_style = NOT_RENDERED;
        menu->styles[i].style = style;
    }

    return menu;
}

// Render a single button of a menu.
// Always performs redraw, does not rotate styling states.
static void render_text_menu_button_nc(text_menu *menu, uint8_t i) {
    const text_menu_template *template = menu->template;
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
    buffered_styling *styles = menu->styles;

    uint8_t i, len = menu->template->len;
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

void del_text_menu(text_menu *menu) {
    free(menu->styles);
    free(menu);
}

basic_text_menu *new_basic_text_menu(const text_menu_template *tmplt, uint8_t s_style, uint8_t ds_style) {
    text_menu *super = new_text_menu(tmplt, ds_style);

    basic_text_menu *bt_menu = safe_malloc(sizeof(basic_text_menu));
    bt_menu->super = super;
    bt_menu->selection_style = s_style;
    bt_menu->deselection_style = ds_style;

    // Defualt to first option selected.
    bt_menu->selection = 0;
    super->styles[0].style = s_style;
}

uint8_t update_basic_text_menu(basic_text_menu *bt_menu) {
    uint8_t format = bt_menu->super->template->format;

    if (
        ((format == MENU_VERTICAL && (key_press(c_Up) || key_press(c_8))) ||
        (format == MENU_HORIZONTAL && (key_press(c_Left) || key_press(c_4)))) &&
        bt_menu->selection != 0 
    ) {
        bt_menu->super->styles[bt_menu->selection].style = bt_menu->deselection_style;
        bt_menu->super->styles[--(bt_menu->selection)].style = bt_menu->selection_style;

        return 1;
    }
    
    if (
        ((format == MENU_VERTICAL && (key_press(c_Down) || key_press(c_5))) ||
        (format == MENU_HORIZONTAL && (key_press(c_Right) || key_press(c_6)))) &&
        bt_menu->selection != bt_menu->super->template->len - 1
    ) {
        bt_menu->super->styles[bt_menu->selection].style = bt_menu->deselection_style;
        bt_menu->super->styles[++(bt_menu->selection)].style = bt_menu->selection_style;

        return 1;
    }

    return 0;
}

void del_basic_text_menu(basic_text_menu *bt_menu) {
    del_text_menu(bt_menu->super);
    free(bt_menu);
}

