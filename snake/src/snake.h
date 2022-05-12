#ifndef SNAKE_H
#define SNAKE_H

#include <stdint.h>

// This is used in the beginning of the game
// when the first block of the snake has no direction.
#define STILL   0

#define NORTH   1
#define SOUTH   2 
#define EAST    3
#define WEST    4

#define vertical(d) d < EAST
#define horizontal(d) d >= EAST


// This will hold the last error thrown by this file.
char *sg_err_message = NULL;

typedef struct game_coordinate {
    uint16_t x;
    uint8_t y;
} coord;

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

typedef struct snake_game_info {
    snake_seg *first;
    snake_seg *last;

    // The amount of food eaten by snake, but not
    // added onto the tail yet.
    uint16_t food_q;

    // Coordinates of food.
    coord food_pos;

    // Cell x Cell size of the environment.
    coord env_dims;

    // Cell size... this is only for rendering.
    uint8_t cell_size;
} snake_game;

snake_game *new_snake_game(uint8_t cs);
void destroy_snake_game(snake_game *sg);

#endif