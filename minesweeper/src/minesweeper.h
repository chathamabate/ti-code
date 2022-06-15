#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdint.h>

typedef struct {
    // A board can be no larger than 255 by 255 cells.
    uint8_t grid_width;
    uint8_t grid_height;    

    // ... And can have no more than 255 mines.
    uint8_t mines;

    // These restrictions are sufficient for the desired
    // difficulties.
} difficulty;

extern const difficulty EASY, MEDIUM, HARD;

#endif
