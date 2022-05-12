#include "snake.h"

#include <stdlib.h>
#include <tice.h>

// Randomize coordinates.
#define rCoord(coord, env) \
    coord.x = rand() % env.x; \
    coord.y = rand() % env.y;

// o - origin.
// d - direction.
// l - length.
// c - coordinate to check.
#define contains(o, d, l, c) \
    (vertical(d) && o.x == c.x && o.y <= c.y && o.y + l >= c.y) || \
    (horizontal(d) && o.y == c.y && o.x <= c.x && o.x + l >= c.x)

// Check to see if a coordinate is contained in a snake segment.
// ss should be a snake_seg *, and c should be a coord.
#define ss_contains(ss, c) \
    contains(ss->pos, ss->direction, ss->size, c)


// Returns 1, if c is part of the snakes tail.
// Returns 0, otherwise.
static uint8_t is_on_snake(snake_game *sg, coord c);

// Randomly generates new food location for the game.
static void respawn_food(snake_game *sg);

snake_game *new_snake_game(uint8_t cs) {
    snake_game *sg = malloc(sizeof(snake_game));

    sg->cell_size = cs;

    if (LCD_WIDTH % cs != 0 || LCD_HEIGHT % cs != 0) {
        sg_err_message = "Invalid cell size given!";    
        return NULL; 
    }

    sg->env_dims.x = (LCD_WIDTH / cs) - 2;
    sg->env_dims.y = (LCD_HEIGHT / cs) - 2;

    if (sg->env_dims.x < 3 || sg->env_dims.y < 3) {
        sg_err_message = "Cell size too large!";
        return NULL;
    }

    snake_seg *first_seg = malloc(sizeof(snake_seg));

    first_seg->next = NULL;
    first_seg->prev = NULL;
    first_seg->direction = STILL;
    first_seg->size = 1;
    rCoord(first_seg->pos, sg->env_dims);

    sg->first = first_seg;
    sg->last = first_seg;

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
    do {
        rCoord(sg->food_pos, sg->env_dims);
    } while (is_on_snake(sg, sg->food_pos));
}


