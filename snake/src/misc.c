#include "misc.h"

#include <graphx.h>
#include <string.h>

menu *new_menu(button *bts, uint8_t bts_len) {
    menu *m = malloc(sizeof(menu));

    m->choice = 0;
    m->buttons = bts;
    m->buttons_len = bts_len;

    // Calc width and height... 
    m->height = (bts_len * (8 * MENU_TXT_H_SCALE)) + ((bts_len + 1) *  MENU_BORDER);

    uint8_t i;
    uint8_t max_width = 0;

    for (i = 0; i < bts_len; i++) {
        bts[i]._text_width = (8 * MENU_TXT_W_SCALE) * strlen(bts[i].text);

        if (bts[i]._text_width > max_width) {
            max_width = bts[i]._text_width;
        }
    }

    m->width = max_width + (2 * MENU_BORDER);

    return m;
}

void render_menu(menu *m, coord c) {
    gfx_SetColor(MENU_BG_COLOR);
    gfx_FillRectangle(c.x, c.y, m->width, m->height);

    gfx_SetMonospaceFont(8); 
    gfx_SetTextScale(MENU_TXT_W_SCALE, MENU_TXT_H_SCALE);
    
    uint16_t cur_x;
    uint8_t i, cur_y = c.y + MENU_BORDER;
    for (i = 0; i < m->buttons_len; i++, cur_y += (8 * MENU_TXT_H_SCALE) + MENU_BORDER) {
        cur_x = (c.x + ((m->width - m->buttons[i]._text_width) / 2));

        gfx_SetTextFGColor(i == m->choice ? MENU_CH_COLOR : MENU_FG_COLOR);
        gfx_PrintStringXY(m->buttons[i].text, cur_x, cur_y);
    }
}

void left_align(char *buff, uint16_t buff_len, char *str, uint16_t str_len, char fill) {
    uint16_t i;
    for (i = 0; i < buff_len; i++) {
        buff[i] = i < str_len ? str[i] : fill;
    }
}

void right_align(char *buff, uint16_t buff_len, char *str, uint16_t str_len, char fill) {
    uint16_t i, len_diff = buff_len - str_len; // Assumes buff_len > str_len.
    for (i = 0; i < buff_len; i++) {
        buff[i] = i < len_diff ? fill : str[i - len_diff];
    }
}

uint8_t dec_digits(uint16_t num) {
    uint8_t digits = 1;
    uint8_t bound = 10;

    while (num >= bound && digits < 5) {
        digits++;
        bound *= 10;
    }

    return digits;
}
