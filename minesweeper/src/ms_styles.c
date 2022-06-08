#include "ms_styles.h"

#include "gfx/borders.h"

const cgfx_pane_style PANE_STYLE_0 = {
    .border_sprites = borders_tiles,
    .fill = 0x07,
    .text_fg = 0x09 
};

const cgfx_pane_style PANE_STYLE_1 = {
    .border_sprites = &borders_tiles[8],
    .fill = 0x0E,
    .text_fg = 0x09
};

const cgfx_pane_style PANE_STYLE_2 = {
    .border_sprites = &borders_tiles[16],
    .fill = 0x06,
    .text_fg = 0x09
};

#define PANE_STYLE_PALETTE_LEN 3

const cgfx_pane_style *PANE_STYLE_PALETTE_DATA[PANE_STYLE_PALETTE_LEN] = {
    &PANE_STYLE_0,
    &PANE_STYLE_1,
    &PANE_STYLE_2
};


