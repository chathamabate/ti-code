#ifndef MS_STYLES_H
#define MS_STYLES_H

#include <cutil/cgraphx.h>
#include <cutil/menu.h>

#define LIGHT_BLUE  0
#define GOLD        1
#define BLACK       2
#define RED         3

extern const cgfx_pane_style PANE_STYLE_0;
extern const cgfx_pane_style PANE_STYLE_1;
extern const cgfx_pane_style PANE_STYLE_2;  
extern const cgfx_pane_style PANE_STYLE_3;

#define PANE_STYLE_PALETTE_LEN 4

extern const cgfx_pane_style *PANE_STYLE_PALETTE_DATA[PANE_STYLE_PALETTE_LEN];

#define PANE_STYLE_PALETTE ((const cgfx_pane_style **)PANE_STYLE_PALETTE_DATA)

extern const selection_styling MS_MENU_SS;

#endif
