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
void reset_ms_game(ms_game *game);

// Delete a new minesweeper game.
void del_ms_game(ms_game *game);

// NOTE, all below integral dimensions are in terms of
// of 16 x 16 tiles.

// This type of cell is used just for rendering.
// Keeps track of the render state of a single cell.
typedef struct {
    // 0 - 8 For each of the cell types.
    uint8_t bg : 4; 

    // 0 For X.
    // 1 - 8 For numbers.
    // 9 For MINE.
    // 10 for no render.
    uint8_t fg : 4;  
} ms_visual_cell;

#define FG_NO_RENDER 10

// Again just for keeping track of state for each cell.
// Since the calc actually runs pretty fast...
// This should be OK.
typedef struct {
    ms_visual_cell buffer_vc;
    ms_visual_cell screen_vc;
    ms_visual_cell actual_vc;
} ms_buffered_visual_cell;

typedef struct {
    // Width and height of window.
    uint8_t w_width, w_height;

    // Selection area of the window
    // relative to the full window. 
    uint8_t s_x_offset, s_y_offset;
    uint8_t s_width, s_height;
} ms_window_template;

// Struct for viewing and interacting with
// a minesweeper game.
typedef struct {
    const ms_window_template *tmplt;

    ms_game *ms_game;

    // Position of window...
    // IN THE MINESWEEPER GRID! 
    uint8_t w_x, w_y;

    // Cursor position in the selection area.
    uint8_t c_x, c_y;

    // The state of all renderable cell locations.
    ms_buffered_visual_cell **render;
} ms_window;


// void render_ms_window_nc

#endif
