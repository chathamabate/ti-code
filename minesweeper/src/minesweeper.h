#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <tice.h>
// #include <stdint.h>

typedef struct {
    uint24_t width;

    // Height is in terms of the board (not the screen).
    // Thus, it can be larger than 255.
    uint24_t height;    

    uint24_t mines;
} difficulty;

extern const difficulty BEGINNER, INTERMEDIATE, EXPERT;

#define SMALL_CELL_SIZE 8
#define LARGE_CELL_SIZE 16

#endif