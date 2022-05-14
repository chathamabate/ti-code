
#include "snake_render.h"

#include <stdio.h>
#include <graphx.h>
#include <tice.h>

#include "misc.h"

#define to_screen_coord(c, sg) \
    (c.x + 1) * sg->cell_size, (c.y + 1) * sg->cell_size

/* Prints a screen centered string */
void print_centered(const char *str) {
    gfx_PrintStringXY(str,
                      (LCD_WIDTH - gfx_GetStringWidth(str)) / 2,
                      (LCD_HEIGHT - 8) / 2);
}

void render_snake_game(snake_game *sg) {
    gfx_FillScreen(255);

    // if (sg->game_state == DEFEAT) {
    //     gfx_SetColor(FOOD_COLOR);
    //     print_centered("DEFEAT");
    // }

    gfx_SetColor(BORDER_COLOR);
    draw_borders(sg);

    gfx_SetColor(FOOD_COLOR);
    gfx_FillRectangle(
        to_screen_coord(sg->food_pos, sg), 
        sg->cell_size, 
        sg->cell_size
    );

    gfx_SetColor(SNAKE_COLOR);

    snake_seg *iter = sg->first;
    while (iter) {
        if (vertical(iter->direction)) {
            gfx_FillRectangle(
                to_screen_coord(iter->pos, sg), 
                sg->cell_size, 
                iter->size * sg->cell_size
            );
        } else {
            gfx_FillRectangle(
                to_screen_coord(iter->pos, sg), 
                iter->size * sg->cell_size,
                sg->cell_size
            );
        }

        iter = iter->next;
    }
}

void draw_borders(snake_game *sg) {
    gfx_FillRectangle(0, 0, LCD_WIDTH, sg->cell_size);
    gfx_FillRectangle(0, LCD_HEIGHT - sg->cell_size, LCD_WIDTH, sg->cell_size);
    gfx_FillRectangle(0, sg->cell_size, sg->cell_size, LCD_HEIGHT - sg->cell_size);
    gfx_FillRectangle(LCD_WIDTH - sg->cell_size, sg->cell_size, sg->cell_size, LCD_HEIGHT - sg->cell_size);
}