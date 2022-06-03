#include <stdio.h>
#include <tice.h>
#include <graphx.h>
#include "minesweeper.h"

#include "cutil/palette.h"

// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

int main(void) {

    // Display size is 320 x 240...
    // What should the minesweeper game modes be...
    // Also how does displaying sprites work...

    gfx_Begin();

    
    uint8_t x = LIGHT_BLUE;
    
    
    
    // White out screen.
    gfx_FillScreen(255);

    const uint8_t CELL_SIZE = 16;

    uint16_t p;
    for (p = 0; p < LCD_WIDTH; p += CELL_SIZE) {
        gfx_SetColor(0);
        gfx_Line(p, 0, p, LCD_HEIGHT);

        gfx_SetColor(120);
        gfx_PrintStringXY("A", p + 2, CELL_SIZE * 4 + 2);
    }

    for (p = 0; p < LCD_HEIGHT; p += CELL_SIZE) {
        gfx_Line(0, p, LCD_WIDTH, p);
    }

    while (os_GetCSC() != sk_Clear);

    gfx_End();

    return 0;
}