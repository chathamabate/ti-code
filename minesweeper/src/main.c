#include <stdio.h>
#include <tice.h>
#include <graphx.h>
#include <cutil/misc.h>
#include <keypadc.h>

#include "minesweeper.h"


// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

int main(void) {

    // Display size is 320 x 240...
    // What should the minesweeper game modes be...
    // Also how does displaying sprites work...

    // This works boiii!!!
    mf_routine = graphics_malloc_fail_routine;

    gfx_SetDrawBuffer();
    gfx_Begin();
    // void *ptr = safe_malloc(100);
    
    // White out screen.
    gfx_FillScreen(255);

    const uint8_t CELL_SIZE = 16;
    gfx_SetColor(0);

    uint16_t p;
    for (p = 0; p < LCD_WIDTH; p += CELL_SIZE) {
        gfx_Line(p, 0, p, LCD_HEIGHT);
    }

    for (p = 0; p < LCD_HEIGHT; p += CELL_SIZE) {
        gfx_Line(0, p, LCD_WIDTH, p);
    }

    gfx_FillRectangle(0, 0, LCD_WIDTH, 2 * CELL_SIZE);
    gfx_FillRectangle(0, LCD_HEIGHT - 2 * CELL_SIZE, LCD_WIDTH, LCD_HEIGHT - 2 * CELL_SIZE);

    gfx_FillRectangle(0, CELL_SIZE, 2 * CELL_SIZE, LCD_HEIGHT - 2 * CELL_SIZE);
    gfx_FillRectangle(LCD_WIDTH - 2 * CELL_SIZE, CELL_SIZE, LCD_WIDTH - 2 * CELL_SIZE, LCD_HEIGHT - 2 * CELL_SIZE);


    gfx_SwapDraw();

    while (1) safe_malloc(1024);

    while (!kb_IsDown(kb_Clear));

    gfx_End();
    exit(1);

    return 0;
}