
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#include "snake.h"
#include "snake_render.h"

void print_centered(const char *str);

int main(void) {
    srand(rtc_Time()); // Timing here could be important.

    snake_game *sg = new_snake_game(8);

    char dimString[50];
    sprintf(dimString, "%d x %d, F=(%d,%d) ", sg->env_dims.x, sg->env_dims.y, sg->food_pos.x, sg->food_pos.y);

    gfx_Begin();
    gfx_SetDrawBuffer();

    render_snake_game(sg);

    gfx_SwapDraw();
    while (!kb_IsDown(kb_KeyClear)) kb_Scan();

    destroy_snake_game(sg);

    goto EXIT;

HANDLE_ERROR:
    // gfx_FillScreen(255);

    // gfx_SetColor(BORDER_COLOR);
    // draw_borders();

    // gfx_SetColor(0);
    // print_centered(err_message ? err_message : "Error!");

    // gfx_SwapDraw();

    // while (!kb_IsDown(kb_KeyClear)) kb_Scan();

EXIT:
    gfx_End(); 
    return 0;
}


/* Prints a screen centered string */
void print_centered(const char *str) {
    gfx_PrintStringXY(str,
                      (LCD_WIDTH - gfx_GetStringWidth(str)) / 2,
                      (LCD_HEIGHT - 8) / 2);
}

// void draw_borders(void) {
//     gfx_FillRectangle(0, 0, LCD_WIDTH, CELL_SIZE);
//     gfx_FillRectangle(0, LCD_HEIGHT - CELL_SIZE, LCD_WIDTH, CELL_SIZE);
//     gfx_FillRectangle(0, CELL_SIZE, CELL_SIZE, LCD_HEIGHT - CELL_SIZE);
//     gfx_FillRectangle(LCD_WIDTH - CELL_SIZE, CELL_SIZE, CELL_SIZE, LCD_HEIGHT - CELL_SIZE);
// }