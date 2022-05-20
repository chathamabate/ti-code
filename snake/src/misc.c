#include "misc.h"

#include <graphx.h>
#include <string.h>

void install_menu(menu *m, button *bts, uint8_t bts_len) {
    m->choice = 0;
    m->buttons = bts;
    m->buttons_len = bts_len;

    gfx_SetTextScale(1, 1);

    // Calc width and height... 

    m->height = (bts_len * (8 * MENU_TXT_H_SCALE)) + ((bts_len + 1) *  MENU_BORDER);

    uint8_t i;
    uint8_t max_width = 0;

    for (i = 0; i < bts_len; i++) {
        bts[i]._text_width = MENU_TXT_W_SCALE * gfx_GetStringWidth(bts[i].text);

        if (bts[i]._text_width > max_width) {
            max_width = bts[i]._text_width;
        }
    }

    m->width = max_width + (2 * MENU_BORDER);
}

void render_menu_xy(menu *m, uint16_t x, uint8_t y) {
    gfx_SetColor(COLOR_4);
    gfx_FillRectangle(x, y, m->width, m->height);

    gfx_SetTextScale(MENU_TXT_W_SCALE, MENU_TXT_H_SCALE);
    
    uint16_t cur_x;
    uint8_t i, cur_y = y + MENU_BORDER;
    for (i = 0; i < m->buttons_len; i++, cur_y += (8 * MENU_TXT_H_SCALE) + MENU_BORDER) {
        cur_x = (x + ((m->width - m->buttons[i]._text_width) / 2));

        gfx_SetTextFGColor(i == m->choice ? COLOR_2 : COLOR_0);
        gfx_PrintStringXY(m->buttons[i].text, cur_x, cur_y);
    }
}

void render_string_xy_mono(char *str, uint8_t str_len, 
    uint16_t x, uint8_t y, uint8_t x_scale, uint8_t y_scale) {
    char char_buff[2];
    char_buff[1] = '\0';

    gfx_SetTextScale(x_scale, y_scale);

    uint16_t x_cur = x;
    uint8_t i;
    for (i = 0; i < str_len; i++, x_cur += (8 * x_scale)) {
        char_buff[0] = str[i];
        gfx_PrintStringXY(char_buff, x_cur, y);
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
