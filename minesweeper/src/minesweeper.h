#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <stdint.h>
#include <cutil/data.h>

#include "ms_styles.h"

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
#define EXPOSED 1 
#define FLAGGED 2

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

// MS Game states.
#define MS_WIN      0
#define MS_DEFEAT   1
#define MS_IN_PLAY  2

// MS_WAITING For when a game has been created but
// not started. In this state the board has been allocated
// space, but no mines have been placed.
#define MS_WAITING  3

// Max time amount.
#define MS_TIMEOUT 999

typedef struct {
    // Difficulty of the game.
    const ms_difficulty *diff;
    
    // gameboard with dimmensions found
    // in diff.
    ms_cell **board;

    // Stack used for uncvoering cells.
    c_stack *uncover_stack;

    // Number of non exposed cell left.
    uint16_t non_exposed_cells;

    // Number of flags placed by user.
    uint8_t flags_left;

    // Time since game has started.
    uint16_t time_elapsed;

    // State of the game.
    uint8_t game_state;
} ms_game;

// Create a new minesweeper game.
ms_game *new_ms_game(const ms_difficulty *diff);

// Set the game to waiting... remove all mines.
void init_ms_game(ms_game *game);

// Start the game with the first cell to be pressed.
void start_ms_game(ms_game *game, uint8_t r, uint8_t c);

// Uncover the cell at position x, y on the game board.
// Provide a stack, so one doesn't need to be made just
// for this funciton call.
void uncover_ms_cell(ms_game *game, uint8_t r, uint8_t c);

// Delete a new minesweeper game.
void del_ms_game(ms_game *game);

#endif
