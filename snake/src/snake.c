#include "snake.h"

#include <stdlib.h>
#include <tice.h>
#include <graphx.h>
// #include <stdio.h>

// Randomize coordinates.
#define r_coord(c, env) \
    (c).x = rand() % (env).x; \
    (c).y = rand() % (env).y;

#define equ_coord(c1, c2) (((c1).x == (c2).x) && ((c1).y == (c2).y))

// o - origin.
// d - direction.
// l - length.
// c - coordinate to check.
#define contains(o, d, l, c) \
    ((vertical(d) && (o).x == (c).x && (o).y <= (c).y && (c).y - (o).y < l) || \
    (horizontal(d) && (o).y == (c).y && (o).x <= (c).x && (c).x - (o).x < l))

// Check to see if a coordinate is contained in a snake segment.
// ss should be a snake_seg *, and c should be a coord.
#define ss_contains(ss, c) \
    contains((ss)->pos, (ss)->direction, ss->size, c)

#define to_screen_coord(c, sg) \
    ((c).x + 1) * (sg)->cell_size, ((c).y + 1) * (sg)->cell_size

char *sg_err_message = "N/A";

// Returns 1, if c is part of the snakes tail.
// Returns 0, otherwise.
static uint8_t is_on_snake(snake_game *sg, coord c);

// Randomly generates new food location for the game.
static void respawn_food(snake_game *sg);

snake_game *new_snake_game(uint8_t cs) {
    snake_game *sg = malloc(sizeof(snake_game));

    sg->cell_size = cs;
    sg->score = 0;
    sg->food_q = 0;
    sg->in_play = 1; // 0 When game is over.

    if (LCD_WIDTH % cs != 0 || LCD_HEIGHT % cs != 0) {
        sg_err_message = "new_snake_game: Invalid cell size given!";    
        return NULL; 
    }

    sg->env_dims.x = (LCD_WIDTH / cs) - 2;
    sg->env_dims.y = (LCD_HEIGHT / cs) - 2;

    if (sg->env_dims.x == 0 || sg->env_dims.y == 0) {
        sg_err_message = "new_snake_game: Cell size too large!";
        return NULL;
    }

    snake_seg *first_seg = malloc(sizeof(snake_seg));

    first_seg->next = NULL;
    first_seg->prev = NULL;
    first_seg->direction = STILL;
    first_seg->size = 1;
    r_coord(first_seg->pos, sg->env_dims);

    sg->first = first_seg;
    sg->last = first_seg;
    sg->direction = STILL;

    // Randomly place the food.
    respawn_food(sg);

    return sg;
}

void destroy_snake_game(snake_game *sg) {
    snake_seg *iter = sg->first;
    snake_seg *temp;

    while (iter) {
        temp = iter->next;
        free(iter);
        iter = temp;    
    }

    free(sg);
}

static uint8_t is_on_snake(snake_game *sg, coord c) {
    snake_seg *iter = sg->first;

    while (iter) {
        if (ss_contains(iter, c)) {
            return 1;
        }

        iter = iter->next;
    }

    return 0;
}

static void respawn_food(snake_game *sg) {
    // Potentially inefficent method here...
    // Should be ok tho...
    do {
        r_coord(sg->food_pos, sg->env_dims);
    } while (is_on_snake(sg, sg->food_pos));
}

void grow(snake_game *sg) {
    // Don't advance a still snake.
    if (sg->direction == STILL) {
        return;
    }

    snake_seg *head_seg = sg->first;

    // Calc coordinate of the leading cell of the snake.
    coord head_cell;
    switch (head_seg->direction) {
    case SOUTH:
        head_cell.x = head_seg->pos.x;
        head_cell.y = head_seg->pos.y + head_seg->size - 1;
        break;
    case EAST:
        head_cell.x = head_seg->pos.x + head_seg->size - 1;
        head_cell.y = head_seg->pos.y;
        break;
    case NORTH:
    case WEST:
        head_cell = head_seg->pos;
        break;
    }

    uint8_t out_of_bounds;
    coord next_cell = head_cell;

    switch (sg->direction) {
    case NORTH:
        out_of_bounds = head_cell.y == 0;
        next_cell.y--;
        break;
    case SOUTH:
        out_of_bounds = head_cell.y == (sg->env_dims.y - 1);
        next_cell.y++;
        break;
    case WEST:
        out_of_bounds = head_cell.x == 0;
        next_cell.x--;
        break;
    case EAST:
        out_of_bounds = head_cell.x == (sg->env_dims.x - 1);
        next_cell.x++;
        break;
    }

    // Invalid next cell has been found!
    if (out_of_bounds || is_on_snake(sg, next_cell)) {
        sg->in_play = 0;
        return;
    }

    // In this situation a new segment must be created.
    if (sg->direction != head_seg->direction) {
        snake_seg *new_seg = malloc(sizeof(snake_seg));

        new_seg->direction = sg->direction;
        new_seg->pos = next_cell;
        new_seg->size = 1;

        new_seg->prev = NULL;
        new_seg->next = head_seg;
        head_seg->prev = new_seg;

        sg->first = new_seg;
    } else {
        // Otherwise, simply expand current segment.
        head_seg->size++;
        if (head_seg->direction == NORTH || head_seg->direction == WEST) {
            head_seg->pos = next_cell;
        }
    }
    
    // NOTE: It is essential this is at the end.
    // This way when food is responded, next_cell can not be chosen.
    if (equ_coord(next_cell, sg->food_pos)) {
        sg->score++;
        sg->food_q += 5; // Allows for same turn growth... 


        // Make sure to respawn the food! 
        respawn_food(sg);
    }
}

void shrink(snake_game *sg) {
    // Cannot shrink a still snake.
    if (sg->direction == STILL) {
        return;
    }

    // Don't need to shrink here!
    if (sg->food_q > 0) {
        sg->food_q--;
        return;
    }

    snake_seg *tail_seg = sg->last;

    tail_seg->size--;

    if (tail_seg->size == 0) {
        // Remove tail seg from the snake.
        sg->last = tail_seg->prev;
        sg->last->next = NULL;

        free(tail_seg);

        return;
    }

    if (tail_seg->direction == EAST) {
        tail_seg->pos.x++;
    } else if (tail_seg->direction == SOUTH) {
        tail_seg->pos.y++;
    }
}

void render_snake_game(snake_game *sg) {
    gfx_FillScreen(COLOR_5);

    gfx_SetColor(COLOR_1);

    gfx_FillRectangle(0, 0, LCD_WIDTH, sg->cell_size);
    gfx_FillRectangle(0, LCD_HEIGHT - sg->cell_size, LCD_WIDTH, sg->cell_size);
    gfx_FillRectangle(0, sg->cell_size, sg->cell_size, LCD_HEIGHT - sg->cell_size);
    gfx_FillRectangle(LCD_WIDTH - sg->cell_size, sg->cell_size, sg->cell_size, LCD_HEIGHT - sg->cell_size);

    gfx_SetColor(COLOR_2);
    gfx_FillRectangle(
        to_screen_coord(sg->food_pos, sg), 
        sg->cell_size, 
        sg->cell_size
    );

    gfx_SetColor(COLOR_0);

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

