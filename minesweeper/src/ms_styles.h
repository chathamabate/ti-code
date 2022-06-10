#ifndef MS_STYLES_H
#define MS_STYLES_H

#include <cutil/cgraphx.h>
#include <cutil/menu.h>

extern const cgfx_pane_style PANE_STYLE_0;
extern const cgfx_pane_style PANE_STYLE_1;
extern const cgfx_pane_style PANE_STYLE_2;  

#define PANE_STYLE_PALETTE_LEN 3

extern const cgfx_pane_style *PANE_STYLE_PALETTE_DATA[PANE_STYLE_PALETTE_LEN];

#define PANE_STYLE_PALETTE ((const cgfx_pane_style **)PANE_STYLE_PALETTE_DATA)

extern const selection_styling MS_MENU_SS;

#endif