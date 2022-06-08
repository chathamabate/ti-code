#include "menu.h"

#include <graphx.h>
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