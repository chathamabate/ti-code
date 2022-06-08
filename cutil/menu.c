#include "menu.h"

#include <graphx.h>
#include <cutil/cgraphx.h>

void render_text_menu_button_nc(text_menu *menu, uint16_t x, uint8_t y, uint8_t i) {
    cgfx_pane_style *pane_style = menu->style_palette[menu->styles[i]];

    uint16_t x_p;
    uint8_t y_p; 

    if (menu->format == MENU_VERTICAL) {
        x_p = x;
        y_p = y + (i * menu->button_height);
    } else {
        x_p = x + (i * menu->button_width);
        y_p = y;
    }

    cgfx_pane_nc(pane_style, x_p, y_p, menu->button_width, menu->button_height);

    uint16_t label_width = gfx_GetStringWidth(menu->labels[i]);

    gfx_SetTextScale(menu->label_width_scale, menu->label_height_scale);
    gfx_SetTextFGColor(pane_style->text_fg);

    gfx_PrintStringXY(
        menu->labels[i], 
        x_p + ((menu->button_width - label_width) / 2), 
        y_p + ((menu->button_height - menu->label_height_scale*8) / 2)
    );
}

void render_text_menu_nc(text_menu *menu, uint16_t x, uint8_t y) {
    uint8_t i;
    for (i = 0; i < menu->len; i++) {
        render_text_menu_button_nc(menu, x, y, i);
    }
}