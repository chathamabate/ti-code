#ifndef SNAKE_RENDER_H
#define SNAKE_RENDER_H

#include "snake.h"

#define BORDER_COLOR 100
#define FOOD_COLOR 200
#define SNAKE_COLOR 50

void render_snake_game(snake_game *sg);
void draw_borders(snake_game *sg);

#endif