
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

    gfx_Begin();
    gfx_SetDrawBuffer();

    sk_key_t key;

    while (sg->game_state != DEFEAT) {
        key = os_GetCSC();

        switch (key) {
        case sk_Up:
            sg->direction = NORTH;
            break;
        case sk_Down:
            sg->direction = SOUTH;
            break;
        case sk_Left:
            sg->direction = EAST;
            break;
        case sk_Right:
            sg->direction = WEST;
            break;
        case sk_Clear:
            // Force Lose!
            sg->game_state = DEFEAT;
            continue;
        }

        if (sg->first->direction == STILL) {
            sg->first->direction = sg->direction;
        } else if ((sg->first->size > 1 || sg->first->prev) && is_opposite(sg->direction, sg->first->direction)) {
            // Cannot pick opposite direction when snake has size greater than 1.
            sg->direction = sg->first->direction;
        } 

        // Finally...
        grow(sg);

        if (sg->game_state != DEFEAT) {
            shrink(sg);            
        }

        render_snake_game(sg);
        gfx_SwapDraw();

        delay(50);
    }

    render_snake_game(sg);
    gfx_SwapDraw();

    while ((key = os_GetCSC()) != sk_Clear);
    gfx_End(); 

    return 0;
}

// void draw_borders(void) {
//     gfx_FillRectangle(0, 0, LCD_WIDTH, CELL_SIZE);
//     gfx_FillRectangle(0, LCD_HEIGHT - CELL_SIZE, LCD_WIDTH, CELL_SIZE);
//     gfx_FillRectangle(0, CELL_SIZE, CELL_SIZE, LCD_HEIGHT - CELL_SIZE);
//     gfx_FillRectangle(LCD_WIDTH - CELL_SIZE, CELL_SIZE, CELL_SIZE, LCD_HEIGHT - CELL_SIZE);
// }