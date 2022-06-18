
#include "minesweeper.h"
#include <stdlib.h>

#include <cutil/misc.h>
#include <stdint.h>
#include <tice.h>

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

    uint8_t r_p, c_p;
    for (r_p = r == 0 ? 0 : r - 1; r_p <= r + 1; r_p++) {
        // Avoid unsigned overflow!
        if (r == UINT8_MAX && r_p == 0) {
            continue;
        }

        for (c_p = c == 0 ? 0 : c - 1; c_p <= c + 1; c_p++) {
            // Same thing again here.
            if (c == UINT8_MAX && c_p == 0) {
                continue;
            }

            if (game->board[r_p][c_p].type == MINE) {
                mines++;
            }
        }
    }

    // Do not include given cell.
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
