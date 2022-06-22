
#include "minesweeper.h"
#include <graphx.h>
#include <stdlib.h>

#include <cutil/misc.h>
#include <cutil/keys.h>

#include <stdint.h>
#include <tice.h>

#include "cutil/data.h"
#include "gfx/tiles16.h"

const ms_difficulty EASY = {
    .grid_width = 8,
    .grid_height = 8,
    .mines = 10
};

const ms_difficulty MEDIUM = {
    .grid_width = 16,
    .grid_height = 16,
    .mines = 40
};

const ms_difficulty HARD = {
    .grid_width = 30,
    .grid_height = 16,
    .mines = 99
};

ms_game *new_ms_game(const ms_difficulty *diff) {
    ms_game *game = safe_malloc(sizeof(ms_game));

    game->diff = diff;

    // Generate board rows.
    game->board = safe_malloc(sizeof(ms_cell *) * diff->grid_height);

    // Generate board cells for each row.
    uint8_t r, c;
    for (r = 0; r < diff->grid_height; r++) {
        game->board[r] = safe_malloc(sizeof(ms_cell) * diff->grid_width);

        // All hidden, no mines to start.
        for (c = 0; c < diff->grid_width; c++) {
            game->board[r][c].visibility = HIDDEN;
            game->board[r][c].type = 0;
        }
    }

    reset_ms_game(game);

    return game;
}

// Get the number of surrounding mines of a cell.
static uint8_t surrounding_mines(ms_game *game, uint8_t r, uint8_t c) {
    uint8_t mines = 0;

    uint8_t s_c_i;
    uint8_t c_i, c_e;
    uint8_t r_i, r_e;

    r_i = r - 1;
    r_e = r + 1;

    if (r == 0) {
        r_i = 0;
    } 
    
    if (r == game->diff->grid_height - 1) {
        r_e = r;
    }

    s_c_i = c - 1;
    c_e = c + 1;

    if (c == 0) {
        s_c_i = 0;
    } 
    
    if (c == game->diff->grid_width - 1) {
        c_e = c;
    }

    for (; r_i <= r_e; r_i++) {
        for (c_i = s_c_i; c_i <= c_e; c_i++) {
            if (game->board[r_i][c_i].type == MINE) {
                mines++;
            }
        }
    }

    if (game->board[r][c].type == MINE) {
        mines--;
    }
       
    return mines;
}

void reset_ms_game(ms_game *game) {
    uint8_t mines_left = game->diff->mines;

    uint8_t r, c; // Randomly place mines.
    while (mines_left) {
        r = random() % game->diff->grid_height;
        c = random() % game->diff->grid_width; 

        if (game->board[r][c].type == MINE) {
            continue;
        }

        game->board[r][c].type = MINE;
        mines_left--;
    }

    for (r = 0; r < game->diff->grid_height; r++) {
        for (c = 0; c < game->diff->grid_width; c++) {
            // All cells start hidden.
            game->board[r][c].visibility = HIDDEN;

            if (game->board[r][c].type == MINE) {
                continue;
            }

            game->board[r][c].type = surrounding_mines(game, r, c);
        }
    }
}

// Probs could've used this in many places.
// But alas... This structure is just used so
// cooredinates can be stored correctly in a stack.
typedef struct {
    uint8_t r;
    uint8_t c;
} ms_cell_coord;

c_stack *new_ms_cell_stack() {
    // Arbitrary starting capacity.
    return new_c_stack(sizeof(ms_cell_coord), 10);
}

void uncover_ms_cell(ms_game *game, c_stack *s, uint8_t r, uint8_t c) {
    game->board[r][c].visibility = EXPOSED;

    // NOTE Skip the algorithm if uncovered cell is not blank.
    if (game->board[r][c].type > 0) {
        return;
    }

    ms_cell_coord *rt_cell_coord = (ms_cell_coord *)c_stack_push(s);    
    rt_cell_coord->r = r;
    rt_cell_coord->c = c;

    // Current coord.
    ms_cell_coord c_cd;

    // Neighbor coords iteration helper variables.
    uint8_t n_r_i, n_r_e;
    uint8_t n_c_i, n_c_e, s_n_c_i;

    
    ms_cell *n_cell; // Neighbor cell pointer.

    // NOTE only freshly exposed empty cells will be placed in the 
    // stack!
    
    while (!c_stack_empty(s)) {
        c_cd = *(ms_cell_coord *)c_stack_peek(s);
        c_stack_pop(s);

        n_r_i = c_cd.r - 1;
        n_r_e = c_cd.r + 1;

        if (c_cd.r == 0) {
            n_r_i = 0;
        } 
        
        if (c_cd.r == game->diff->grid_height - 1) {
            n_r_e = c_cd.r;
        }

        s_n_c_i = c_cd.c - 1;
        n_c_e = c_cd.c + 1;

        if (c_cd.c == 0) {
            s_n_c_i = 0;
        }

        if (c_cd.c == game->diff->grid_width - 1) {
            n_c_e = c_cd.c;
        }


        for (; n_r_i <= n_r_e; n_r_i++) {
            for (n_c_i = s_n_c_i; n_c_i <= n_c_e; n_c_i++) {
                if (n_r_i == c_cd.r && n_c_i == c_cd.c) {
                    continue; // Skip current cell.
                }

                n_cell = &(game->board[n_r_i][n_c_i]);

                if (n_cell->visibility == EXPOSED) {
                    continue; // Skip already exposed cells.
                }

                // Expose cell if needed.
                game->board[n_r_i][n_c_i].visibility = EXPOSED;

                if (game->board[n_r_i][n_c_i].type == 0) {
                    // Push onto stack if empty cell has been exposed.
                    rt_cell_coord = (ms_cell_coord *)c_stack_push(s); 
                    rt_cell_coord->r = n_r_i;
                    rt_cell_coord->c = n_c_i;
                }
            }
        }
    }
}

void del_ms_game(ms_game *game) {
    // Free each row.
    uint8_t r;
    for (r = 0; r < game->diff->grid_height; r++) {
        free(game->board[r]);
    }

    // Free the full table.
    free(game->board);

    // finally free the game itself.
    free(game);
}

#define INIT_VIS_CELL(v_cell) \
    (v_cell).bg = BG_NO_RENDER; \
    (v_cell).fg = FG_NO_RENDER

#define INIT_BUFF_VIS_CELL(bv_cell) \
    INIT_VIS_CELL((bv_cell).buffer_vc); \
    INIT_VIS_CELL((bv_cell).screen_vc); \
    INIT_VIS_CELL((bv_cell).actual_vc)

// Helper for loading a game entirely into the window.
static void load_ms_window(ms_window *window) {
    // Game coordinates of the window's top left corner.
    int16_t w_r = window->w_r;
    int16_t w_c = window->w_c;

    uint8_t g_width = window->game->diff->grid_width;
    uint8_t g_height = window->game->diff->grid_height;

    const ms_window_template *tmplt = window->tmplt;

    ms_cell **board = window->game->board;
    ms_buffered_visual_cell **render = window->render;

    int16_t r, c;       // These are game coordinates.
    int8_t a_r, a_c;    // These are coordinates into the window. 
    for (r = w_r; r < w_r + tmplt->w_height; r++) {
        for (c = w_c; c < w_c + tmplt->w_width; c++) {
            a_r = r - w_r;
            a_c = c - w_c; 
            
            if (r < 0 || r >= (int16_t)g_height || 
                    c < 0 || c >= (int16_t)g_width) {
                // If the current cell is off the gameboard.
                render[a_r][a_c].actual_vc.bg = BG_BARRIER(BLACK);  
                render[a_r][a_c].actual_vc.fg = FG_NO_RENDER;

                continue;
            } 

            // Otherwise cell is on the gameboard.

            // Flagged -> HIDDEN.
            if (board[r][c].visibility == FLAGGED) {
                render[a_r][a_c].actual_vc.bg = BG_HIDDEN(BLACK);
                render[a_r][a_c].actual_vc.fg = 0; // X is 0.
            } else if (board[r][c].visibility == HIDDEN) {
                render[a_r][a_c].actual_vc.bg = BG_HIDDEN(BLACK);
                render[a_r][a_c].actual_vc.fg = FG_NO_RENDER;
            } else {
                // EXPOSED CASE.
                render[a_r][a_c].actual_vc.bg = BG_EXPOSED(BLACK);
                render[a_r][a_c].actual_vc.fg = 
                    board[r][c].type == 0 
                    ? FG_NO_RENDER : board[r][c].type;
            }
        }
    }

    uint8_t s_r, s_c;
    for (s_r = tmplt->s_r_offset; s_r < tmplt->s_r_offset + tmplt->s_height; s_r++) {
        for (s_c = tmplt->s_c_offset; s_c < tmplt->s_c_offset + tmplt->s_width; s_c++) {
            render[s_r][s_c].actual_vc.bg -= 6; // Black to Light Blue.
        }
    }

    // From light blue to gold.
    render[tmplt->s_r_offset + window->c_r][tmplt->s_c_offset + window->c_c]
        .actual_vc.bg += 3;
}

ms_window *new_ms_window(const ms_window_template *tmplt, ms_game *game) {
    ms_window *window = safe_malloc(sizeof(ms_window));

    window->tmplt = tmplt;
    window->game = game;
    window->s = new_ms_cell_stack();

    // Unsure if cast is really needed here.
    window->w_r = -(int16_t)(tmplt->s_r_offset);
    window->w_c = -(int16_t)(tmplt->s_c_offset);

    window->c_r = 0;
    window->c_c = 0;

    window->render = (ms_buffered_visual_cell **)
        safe_malloc(sizeof(ms_buffered_visual_cell *) * tmplt->w_height);

    uint8_t r, c;
    for (r = 0; r < tmplt->w_height; r++) {
        window->render[r] = (ms_buffered_visual_cell *)
            safe_malloc(sizeof(ms_buffered_visual_cell) * tmplt->w_width);

        for (c = 0; c < tmplt->w_width; c++) {
            INIT_BUFF_VIS_CELL(window->render[r][c]);
        }
    }

    load_ms_window(window);
    
    return window;
}

uint8_t update_ms_window(ms_window *window) {
    // Should return if any change requiring a re render has occured! 
    // Assume scan has already occured.

    // Screen coordinates of cursor. (in 16s)
    uint8_t screen_c_r = window->tmplt->s_r_offset + window->c_r;
    uint8_t screen_c_c = window->tmplt->s_c_offset + window->c_c;

    uint8_t game_c_r = screen_c_r + window->w_r;
    uint8_t game_c_c = screen_c_c + window->w_c;

    // Uncover logic.
    if (key_press(c_9)) {
        if (window->game->board[game_c_r][game_c_c].visibility == HIDDEN) {
            uncover_ms_cell(window->game, window->s, game_c_r, game_c_c);

            // NOTE Slight optimizations could be put here.
            load_ms_window(window); // Reload screen.
                                    
            return 1;
        }

        return 0;
    }

    // Flagging Logic.

    if (key_press(c_7)) {
        if (window->game->board[game_c_r][game_c_c].visibility == HIDDEN) {
            window->game->board[game_c_r][game_c_c].visibility = FLAGGED;
            window->render[screen_c_r][screen_c_c].actual_vc.fg = 0;

            return 1;
        }
        
        if (window->game->board[game_c_r][game_c_c].visibility == FLAGGED) {
            window->game->board[game_c_r][game_c_c].visibility = HIDDEN;
            window->render[screen_c_r][screen_c_c].actual_vc.fg = FG_NO_RENDER;

            return 1;
        }

        return 0;
    } 

    // Final movement logic.
    
    if (key_press(c_8) || key_press(c_Up)) {
        if (game_c_r == 0) {
            return 0;
        }

        if (window->c_r == 0) {
            window->w_r--;
            // Full screen needs a reload.
            load_ms_window(window);
            return 1;
        }

        window->c_r--;
    } else if (key_press(c_4) || key_press(c_Left)) {
        if (game_c_c == 0) {
            return 0;
        }

        if (window->c_c == 0) {
            window->w_c--;
            load_ms_window(window);
            return 1;
        }

        window->c_c--;
    } else if (key_press(c_5) || key_press(c_Down)) {
        if (game_c_r == window->game->diff->grid_height - 1) {
            return 0;
        }

        if (window->c_r == window->tmplt->s_height - 1) {
            window->w_r++;
            load_ms_window(window);
            return 1;
        }

        window->c_r++;
    } else if (key_press(c_6) || key_press(c_Right)) {
        if (game_c_c == window->game->diff->grid_width - 1) {
            return 0;
        }

        if (window->c_c == window->tmplt->s_width - 1) {
            window->w_c++;
            load_ms_window(window);
            return 1;
        }

        window->c_c++;
    } else {
        return 0;
    }

    // Gold to blue.
    window->render[screen_c_r][screen_c_c].actual_vc.bg -= 3;

    // Blue to gold.
    window->render[window->tmplt->s_r_offset + window->c_r]
        [window->tmplt->s_c_offset + window->c_c].actual_vc.bg += 3;

    return 1;
}

void render_ms_window_nc(ms_window *window) {
    ms_buffered_visual_cell **render = window->render;

    uint8_t rows = window->tmplt->w_height;
    uint8_t cols = window->tmplt->w_width;

    uint16_t x = window->tmplt->x;
    uint8_t y = window->tmplt->y;

    ms_buffered_visual_cell *bv_cell;
    uint8_t r, c;

    uint16_t x_p;
    uint8_t y_p;
    for (r = 0, y_p = y; r < rows; r++, y_p += 16) {
        for (c = 0, x_p = x; c < cols; c++, x_p += 16) {
            bv_cell = &(render[r][c]);

            if (!ms_visual_cell_equ(bv_cell->actual_vc, bv_cell->buffer_vc)) {
                if (bv_cell->actual_vc.bg < BG_NO_RENDER) {
                    gfx_Sprite_NoClip(tiles16_tiles[bv_cell->actual_vc.bg], x_p, y_p);
                }

                if (bv_cell->actual_vc.fg < FG_NO_RENDER) {
                    gfx_TransparentSprite_NoClip(
                            tiles16_tiles[9 + bv_cell->actual_vc.fg], x_p, y_p);
                }
            } 

            // Swap buffer and screen!
            bv_cell->buffer_vc = bv_cell->screen_vc;
            bv_cell->screen_vc = bv_cell->actual_vc;
        }
    } 
}

void del_ms_window(ms_window *window) {
    uint8_t r;
    for (r = 0; r < window->tmplt->w_height; r++) {
        free(window->render[r]);
    } 

    free(window->render);
    del_c_stack(window->s);
    free(window);

    // NOTE, the game is not freed here!!!!
}
