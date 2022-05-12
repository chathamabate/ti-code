
#include "snake_render.h"

#include <graphx.h>
#include <tice.h>

void render_snake_game(snake_game *sg) {
    gfx_SetColor(BORDER_COLOR);
    draw_borders(sg);

    gfx_SetColor(FOOD_COLOR);
    gfx_FillRectangle(
        (sg->food_pos.x + 1) * sg->cell_size, // Don't accidentally draw on the border.
        (sg->food_pos.y + 1) * sg->cell_size, 
        sg->cell_size, 
        sg->cell_size
    );
}

void draw_borders(snake_game *sg) {
    gfx_FillRectangle(0, 0, LCD_WIDTH, sg->cell_size);
    gfx_FillRectangle(0, LCD_HEIGHT - sg->cell_size, LCD_WIDTH, sg->cell_size);
    gfx_FillRectangle(0, sg->cell_size, sg->cell_size, LCD_HEIGHT - sg->cell_size);
    gfx_FillRectangle(LCD_WIDTH - sg->cell_size, sg->cell_size, sg->cell_size, LCD_HEIGHT - sg->cell_size);
}