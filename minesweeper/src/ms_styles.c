#include "ms_styles.h"

#include <cutil/menu.h>
#include "gfx/borders.h"

const cgfx_pane_style PANE_STYLE_0 = {
    .border_sprites = borders_tiles,
    .fill = 0x07,
    .text_fg = 0x01 
};

const cgfx_pane_style PANE_STYLE_1 = {
    .border_sprites = &borders_tiles[8],
    .fill = 0x0E,
    .text_fg = 0x01
};

const cgfx_pane_style PANE_STYLE_2 = {
    .border_sprites = &borders_tiles[16],
    .fill = 0x06,
    .text_fg = 0x01
};

#define PANE_STYLE_PALETTE_LEN 3

const cgfx_pane_style *PANE_STYLE_PALETTE_DATA[PANE_STYLE_PALETTE_LEN] = {
    &PANE_STYLE_0,
    &PANE_STYLE_1,
    &PANE_STYLE_2
};

const selection_styling MS_MENU_SS = {
    .deselection_style = LIGHT_BLUE,
    .selection_style = GOLD,
    .unfocus_style = BLACK 
};


