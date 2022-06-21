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

// There are 11 cell types.
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

// Create a stack usable for the cell uncover algo.
c_stack *new_ms_cell_stack();

// Uncover the cell at position x, y on the game board.
// Provide a stack, so one doesn't need to be made just
// for this funciton call.
void uncover_ms_cell(ms_game *game, c_stack *s, uint8_t r, uint8_t c);

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

#define BG_HIDDEN(style) ((style) * 3)
#define BG_EXPOSED(style) (((style) * 3) + 1)
#define BG_BARRIER(style) (((style) * 3) + 2)
#define BG_NO_RENDER 10 

#define FG_NO_RENDER 9 

#define ms_visual_cell_equ(v_cell_1, v_cell_2) \
    ((v_cell_1).bg == (v_cell_2).bg && (v_cell_1).fg == (v_cell_2).fg) 

// Again just for keeping track of state for each cell.
// Since the calc actually runs pretty fast...
// This should be OK.
typedef struct {
    ms_visual_cell buffer_vc;
    ms_visual_cell screen_vc;
    ms_visual_cell actual_vc;
} ms_buffered_visual_cell;

typedef struct {
    // Real on screen coordinates.
    uint16_t x; 
    uint8_t y;

    // Width and height of window.
    uint8_t w_width, w_height;

    // Selection area of the window
    // relative to the full window. 
    uint8_t s_r_offset, s_c_offset;
    uint8_t s_width, s_height;
} ms_window_template;

// Struct for viewing and interacting with
// a minesweeper game.
typedef struct {
    const ms_window_template *tmplt;
    ms_game *game; // NOTE, this is independent of the window.

    // Position of window...
    // IN THE MINESWEEPER GRID! 
    int16_t w_r, w_c; // These can be negative!!

    // Cursor position in the selection area.
    uint8_t c_r, c_c; 

    // The state of all renderable cell locations.
    ms_buffered_visual_cell **render;
} ms_window;

// Create a window into some game.
ms_window *new_ms_window(const ms_window_template *tmplt, ms_game *game);

// Returns whether or not any state changed.
uint8_t update_ms_window(ms_window *window);

// Render the window.
void render_ms_window_nc(ms_window *window);

// NOTE, this does not delete the game the window looks into.
void del_ms_window(ms_window *window);

#endif
