#include "misc.h"

#include <graphx.h>
#include <fontlibc.h>

#include <tice.h>
#include <stdio.h>

menu *new_menu(button *bts, uint8_t bts_len) {
    menu *m = malloc(sizeof(menu));

    m->choice = 0;
    m->buttons = bts;
    m->buttons_len = bts_len;

    // Calc width and height... 
    m->height = bts_len * fontlib_GetCurrentFontHeight();

    uint8_t i, temp_width;
    uint8_t max_width = 0;

    for (i = 0; i < bts_len; i++) {
        temp_width = MENU_TXT_WIDTH * // TODO FIX THIS!!!!!!!

        if (temp_width > max_width) {
            max_width = temp_width;
        }
    }

    m->width = max_width;

    // char buff[20];
    // sprintf(buff, "%d", max_width);
    // os_ClrHome();
    // os_PutStrFull(buff);
    // os_NewLine();

    return m;
}

void render_menu(menu *m, coord c) {
    gfx_SetColor(MENU_BG_COLOR);
    gfx_FillRectangle(c.x, c.y, m->width, m->height);

    gfx_Scale
    gfx_FillRectangle(c.x, c.y, 10, m->height);

    // gfx_SetColor(MENU_FG_COLOR);

    // uint8_t cur_x, cur_y = c.y, i, skip = fontlib_GetCurrentFontHeight();
    // for (i = 0; i < m->buttons_len; i++, cur_y += skip) {
    //     cur_x = c.x + ((m->width - fontlib_GetStringWidth(m->buttons[i].text)) / 2);

    //     gfx_SetColor(i == m->choice ? MENU_CH_COLOR : MENU_FG_COLOR);
    //     gfx_PrintStringXY(m->buttons[i].text, cur_x, cur_y);
    // }
}