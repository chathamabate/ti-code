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
} ms_difficulty;

extern const ms_difficulty EASY, MEDIUM, HARD;

// Visibilities.
#define HIDDEN  0
#define FLAGGED 1
#define EXPOSED 2

// There are 10 cell types.
// 0 - 8 refer to the number of
// bordering mines.
// 9 is if the cell itself contains 
// a mine.
#define MINE 9

typedef struct {
    uint8_t visibility : 2;
    uint8_t type : 4;
} ms_cell;

typedef struct {
    // Difficulty of the game.
    const ms_difficulty *diff;
    
    // gameboard with dimmensions found
    // in diff.
    ms_cell **board;
} ms_game;

// Create a new minesweeper game.
// NOTE, this calls init_ms_game.
ms_game *new_ms_game(const ms_difficulty *diff);

// Initialize the board.
// i.e. place mines and numbers.
void init_ms_game(ms_game *game);

// Delete a new minesweeper game.
void del_ms_game(ms_game *game);

#endif
