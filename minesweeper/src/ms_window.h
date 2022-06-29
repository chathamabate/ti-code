#ifndef MS_WINDOW_H
#define MS_WINDOW_H

#include "minesweeper.h"
#include <sys/timers.h>

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

// i.e. number of background tiles
#define BG_NO_RENDER 12 

// 0 is X
// 1 - 8 are themx selves.
// 9 is mine.
#define FG_NO_RENDER 10

// Animation delay amount for a vicotry/defeat sequence.
#define MS_WINDOW_ANIMATION_DEL 5 

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
    ms_game *game; 
    uint8_t paused; // this will be a feature of the window
                    // not the game.
    
    // Position of window...
    // IN THE MINESWEEPER GRID! 
    int16_t w_r, w_c; // These can be negative!!

    // Cursor position in the selection area.
    uint8_t c_r, c_c; 

    // To be used when game has been won or lost.
    uint8_t animation_frame;
    uint8_t animation_tick;

    // The state of all renderable cell locations.
    ms_buffered_visual_cell **render;
} ms_window;

// Create the memory for the window.
ms_window *new_ms_window(const ms_window_template *tmplt, uint8_t d_i);

// Set window up to be used.
void init_ms_window(ms_window *window);

// Macro for pausing window.
// Should only be used when game is in play or waiting.
#define pause_ms_window(window) \
    timer_Disable(1); \
    (window)->paused = 1; \
    reset_render_ms_window(window)

// Should only be used when game is paused.
#define resume_ms_window(window) \
    (window)->paused = 0; \
    timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN)

// Returns whether or not any state changed.
uint8_t update_ms_window(ms_window *window);

// Render the window.
void render_ms_window_nc(ms_window *window);

// Clear window's render memory.
void reset_render_ms_window(ms_window *window);

// NOTE, this does not delete the game the window looks into.
void del_ms_window(ms_window *window);

#endif
