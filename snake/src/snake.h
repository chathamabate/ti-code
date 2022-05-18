#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>
#include "misc.h"

// This is used in the beginning of the game
// when the first block of the snake has no direction.
#define STILL   0

#define NORTH   1
#define EAST    2
#define SOUTH   3
#define WEST    4

#define vertical(d) (((d) % 2) == 1)
#define horizontal(d) (((d) % 2) == 0)
#define opposite(d) ((((d) + 1) % 4) + 1)
#define is_opposite(d1, d2) ((d1) == opposite(d2))

// This will hold the last error thrown by this file.
extern char *sg_err_message;

typedef struct snake_segment {
    coord pos;

    uint16_t size;

    // Denotes the direction to grow the segment.
    // Opposite of shrink direction.
    uint8_t direction;

    // Snake segments form a doubly linked list.
    struct snake_segment *next;
    struct snake_segment *prev;
} snake_seg;

typedef struct {
    snake_seg *first;
    snake_seg *last;

    // The direction to move the snake in next advance.
    uint8_t direction;

    // The amount of food eaten by snake, but not
    // added onto the tail yet.
    uint16_t food_q;

    // The player's score so far.
    // +1 for each food eaten.
    uint16_t score;

    // Coordinates of food.
    coord food_pos;

    // Cell x Cell size of the environment.
    coord env_dims;

    // Cell size... this is only for rendering.
    uint8_t cell_size;

    uint8_t in_play;
} snake_game;

snake_game *new_snake_game(uint8_t cs);
void destroy_snake_game(snake_game *sg);

// Grow the snake in its pointed direction.
void grow(snake_game *sg);

// Shrink the tail of the snake (if needed)
void shrink(snake_game *sg);

// Render the snake game.
void render_snake_game(snake_game *sg);

#endif