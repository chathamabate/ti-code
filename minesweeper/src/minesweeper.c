
#include "minesweeper.h"
#include <graphx.h>
#include <stdlib.h>

#include <cutil/misc.h>

#include <stdint.h>
#include <tice.h>

#include "cutil/data.h"
#include "ms_mem_channels.h"

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

// Probs could've used this in many places.
// Probs could've used this in many places.
// But alas... This structure is just used so
// cooredinates can be stored correctly in a stack.
typedef struct {
    uint8_t r;
    uint8_t c;
} ms_cell_coord;

ms_game *new_ms_game(const ms_difficulty *diff) {
    ms_game *game = safe_malloc(MS_GAME_CHANNEL, sizeof(ms_game));

    game->diff = diff;

    // Generate board rows.
    game->board = safe_malloc(MS_GAME_CHANNEL, sizeof(ms_cell *) * diff->grid_height);

    // Generate board cells for each row.
    uint8_t r;
    for (r = 0; r < diff->grid_height; r++) {
        game->board[r] = safe_malloc(MS_GAME_CHANNEL, sizeof(ms_cell) * diff->grid_width);
    }

    game->uncover_stack = new_c_stack(sizeof(ms_cell_coord), 10);

    return game;
}

void init_ms_game(ms_game *game) {
    uint8_t r, c;
    for (r = 0; r < game->diff->grid_height; r++) {
        for (c = 0; c < game->diff->grid_width; c++) {
            game->board[r][c].visibility = HIDDEN;
            game->board[r][c].type = 0;
        }
    }

    game->game_state = MS_WAITING;
    game->time_elapsed = 0;
    game->flags_left = game->diff->mines;
    game->non_exposed_cells = (uint16_t)game->diff->grid_width * 
        (uint16_t)game->diff->grid_height;
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

void start_ms_game(ms_game *game, uint8_t r, uint8_t c) {
    uint8_t mines_left = game->diff->mines;

    uint8_t t_r, t_c; // Randomly place mines.
    int16_t r_diff, c_diff;
    while (mines_left) {
        t_r = random() % game->diff->grid_height;
        t_c = random() % game->diff->grid_width; 

        if (game->board[t_r][t_c].type == MINE) {
            continue;
        }

        // Must skip anytthing which borders (r, c)
        // if abs(t_r - r) <= 1 || abs(t_c - c) <= 1
        r_diff = ((int16_t)r + 1) - (int16_t)t_r;
        c_diff = ((int16_t)c + 1) - (int16_t)t_c;

        if (r_diff >= 0 && r_diff <= 2 && c_diff >= 0 && c_diff <= 2) {
            continue;
        }

        game->board[t_r][t_c].type = MINE;
        mines_left--;
    }

    for (t_r = 0; t_r < game->diff->grid_height; t_r++) {
        for (t_c = 0; t_c < game->diff->grid_width; t_c++) {
            if (game->board[t_r][t_c].type == MINE) {
                continue;
            }

            game->board[t_r][t_c].type = surrounding_mines(game, t_r, t_c);
        }
    }

    // Uncover pressed cell.
    uncover_ms_cell(game, r, c);

    // Start the game!
    game->game_state = MS_IN_PLAY;

    // TODO Timing.
}



void uncover_ms_cell(ms_game *game, uint8_t r, uint8_t c) {
    game->board[r][c].visibility = EXPOSED;
    game->non_exposed_cells--;

    // NOTE Skip the algorithm if uncovered cell is not blank.
    if (game->board[r][c].type > 0) {
        return;
    }

    c_stack *s = game->uncover_stack;

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

                // Incorrectly flagged cells will be exposed here
                // just to be nice.
                if (n_cell->visibility == FLAGGED) {
                    game->flags_left++;
                }

                // Expose cell if needed.
                game->board[n_r_i][n_c_i].visibility = EXPOSED;
                game->non_exposed_cells--;

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
        safe_free(MS_GAME_CHANNEL, game->board[r]);
    }

    // Free the full table.
    safe_free(MS_GAME_CHANNEL, game->board);

    // Delete the uncover stack.
    del_c_stack(game->uncover_stack);

    // finally free the game itself.
    safe_free(MS_GAME_CHANNEL, game);
}


