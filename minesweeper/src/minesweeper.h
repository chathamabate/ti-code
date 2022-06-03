#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdint.h>

typedef struct {
    // A board can be no larger than 255 by 255 cells.
    uint8_t width;
    uint8_t height;    

    // ... And can have no more than 255 mines.
    uint8_t mines;

    // These restrictions are sufficient for the desired
    // difficulties.
} difficulty;

extern const difficulty BEGINNER, INTERMEDIATE, EXPERT;

#define HIDDEN  0 
#define FLAGGED 1
#define EXPOSED 2

// 0 - 8  will be the number of mines bordering the cell.
// This number will remain constant througout the game.
#define MINE 9

typedef struct {
    uint8_t interaction : 2;

    // Bordering mines... or if the cell is a mine.
    uint8_t state : 4;
} cell;

typedef struct {
    const difficulty *diff;

    // Number of cells on the board which are exposed.
    // This makes checking if the game is over easy.
    uint24_t exposed_cells;

    cell **board;
} game;



#endif