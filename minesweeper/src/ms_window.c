#include "ms_window.h"

#include <graphx.h>
#include <stdlib.h>

#include <cutil/misc.h>
#include <cutil/keys.h>

#include <stdint.h>
#include <tice.h>

#include "cutil/data.h"
#include "gfx/tiles16.h"
#include "minesweeper.h"
#include "sys/timers.h"

#include "ms_mem_channels.h"

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

    const ms_difficulty *diff = DIFFS[window->game->diff_ind];

    uint8_t g_width = diff->grid_width;
    uint8_t g_height = diff->grid_height;

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

// Number of frames to wait before switching the window skin
// during an animation.
#define MS_WINDOW_ANIMATION_DEL 5 

// TODO / NOTE
// To consider here... should the window have control over creating the game 
// as well???
// This way the first press by the user can always be a blank space???
// 

ms_window *new_ms_window(const ms_window_template *tmplt, uint8_t d_i) {
    ms_window *window = safe_malloc(MS_WINDOW_CHANNEL, sizeof(ms_window));

    window->tmplt = tmplt;
    window->game = new_ms_game(d_i);

    window->render = (ms_buffered_visual_cell **)
        safe_malloc(MS_WINDOW_CHANNEL, sizeof(ms_buffered_visual_cell *) * tmplt->w_height);

    uint8_t r;
    for (r = 0; r < tmplt->w_height; r++) {
        window->render[r] = (ms_buffered_visual_cell *)
            safe_malloc(MS_WINDOW_CHANNEL, sizeof(ms_buffered_visual_cell) * tmplt->w_width);
    }
    
    return window;
}

void init_ms_window(ms_window *window) {
    const ms_window_template *tmplt = window->tmplt;

    window->paused = 0;

    // Unsure if cast is really needed here.
    window->w_r = -(int16_t)(tmplt->s_r_offset);
    window->w_c = -(int16_t)(tmplt->s_c_offset);

    window->c_r = 0;
    window->c_c = 0;

    window->animation_frame = MS_WINDOW_ANIMATION_DEL;
    window->animation_tick = 0;

    uint8_t r,c;
    for (r = 0; r < tmplt->w_height; r++) {
        for (c = 0; c < tmplt->w_width; c++) {
            INIT_BUFF_VIS_CELL(window->render[r][c]);
        }
    }

    init_ms_game(window->game);
    load_ms_window(window);
}

// NOTE this struct is used for animation stylings.
// All will be indicies into background tiles.
typedef struct {
    uint8_t exposed_bg;
    uint8_t hidden_bg;
    uint8_t mine_bg;
} ms_window_skin;

// Change the appearance of all exposed and hidden cells
// displayed in a window... (Flagged = Hidden in this case)
static void mask_ms_window(ms_window *window, const ms_window_skin *skin) {
    const ms_difficulty *diff = DIFFS[window->game->diff_ind];

    // Iterators for rows and columns in window coordinates.
    uint8_t r_i;
    uint8_t c_i, s_c_i;

    uint16_t r_e, c_e;

    // game coordinate versions.
    uint8_t r, c, s_c;

    r_i = 0;
    if (window->w_r < 0) {
        r_i = (uint8_t)(-window->w_r);
    }

    r_e = (uint16_t)diff->grid_height - (uint16_t)window->w_r;
    if (r_e > window->tmplt->w_height) {
        r_e = window->tmplt->w_height;
    }

    s_c_i = 0;
    if (window->w_c < 0) {
        s_c_i = (uint8_t)(-window->w_c);  
    } 

    c_e = (uint16_t)diff->grid_width - (uint16_t)window->w_c;
    if (c_e > window->tmplt->w_width) {
        c_e = window->tmplt->w_width;
    } 

    // Game coordinate versions of r_i and c_i.
    r = window->w_r + r_i;
    s_c = window->w_c + s_c_i;

    ms_buffered_visual_cell **render = window->render;
    ms_cell **board = window->game->board;

    for (; r_i < r_e; r_i++, r++) {
        for (c_i = s_c_i, c = s_c; c_i < c_e; c_i++, c++) {
            if (board[r][c].type == MINE) {
                render[r_i][c_i].actual_vc.bg = skin->mine_bg;
            } else if (board[r][c].visibility == EXPOSED) {
                render[r_i][c_i].actual_vc.bg = skin->exposed_bg;
            } else {
                render[r_i][c_i].actual_vc.bg = skin->hidden_bg;
            }

            // All game cells are striped of FG.
            render[r_i][c_i].actual_vc.fg = FG_NO_RENDER;
        }
    }
}


static uint8_t animate_ms_window(ms_window *window, 
        const ms_window_skin **skins, uint8_t skins_len) {
    // Advance animation.
    window->animation_tick = (window->animation_tick + 1);

    if (window->animation_tick % MS_WINDOW_ANIMATION_DEL != 0) {
        // No flick needed.
        return 0;
    }

    // Switch frame.
    window->animation_frame = (window->animation_frame + 1) % skins_len;
    mask_ms_window(window, skins[window->animation_frame]);
    return 1;
}

static const ms_window_skin WIN_SKIN_0 = {
    .exposed_bg = BG_EXPOSED(LIGHT_BLUE),
    .hidden_bg = BG_NO_RENDER, // Should never be used in win case.
    .mine_bg = BG_HIDDEN(GOLD)
};

static const ms_window_skin WIN_SKIN_1 = {
    .exposed_bg = BG_EXPOSED(GOLD),
    .hidden_bg = BG_NO_RENDER, // Should never be used in win case.
    .mine_bg = BG_HIDDEN(LIGHT_BLUE)
};

#define WIN_SKINS_LEN 2
static const ms_window_skin *WIN_SKINS[WIN_SKINS_LEN] = {
    &WIN_SKIN_0, &WIN_SKIN_1 
};

static const ms_window_skin DEFEAT_SKIN_0 = {
    .exposed_bg = BG_EXPOSED(BLACK),
    .hidden_bg = BG_HIDDEN(BLACK),
    .mine_bg = BG_HIDDEN(RED)
};

static const ms_window_skin DEFEAT_SKIN_1 = {
    .exposed_bg = BG_EXPOSED(RED),
    .hidden_bg = BG_HIDDEN(RED),
    .mine_bg = BG_HIDDEN(BLACK)
};

#define DEFEAT_SKINS_LEN 2 
static const ms_window_skin *DEFEAT_SKINS[DEFEAT_SKINS_LEN] = {
    &DEFEAT_SKIN_0, &DEFEAT_SKIN_1
};

// Timer frequencies from example.
#define TIMER_FREQ      32768 /* Frequency of timer in Hz */
#define ONE_SECOND      (TIMER_FREQ / 1)
#define HALF_SECOND     (TIMER_FREQ / 2)
#define QUARTER_SECOND  (TIMER_FREQ / 4)

uint8_t update_ms_window(ms_window *window) {
    // NOTE, during win/loss states...
    // no normal game logic registers.
    if (window->game->game_state == MS_WIN) {
        return animate_ms_window(window, WIN_SKINS, WIN_SKINS_LEN);
    }

    if (window->game->game_state == MS_DEFEAT) {
        return animate_ms_window(window, DEFEAT_SKINS, DEFEAT_SKINS_LEN);
    }

    // NOTE we do not check if the window is paused ever.
    // Is the window is paused, it should not be updated
    // until being resumed.

    // Do timer check when the game is in play.
    if (window->game->game_state == MS_IN_PLAY &&
            timer_ChkInterrupt(1, TIMER_RELOADED)) {
        if (window->game->time_elapsed < MS_TIMEOUT) {
            window->game->time_elapsed++;
        }
        timer_AckInterrupt(1, TIMER_RELOADED);
    }

    // If we make it here, must be in play or waiting.
    // Pause logic is appropriate.
    
    if (key_press(c_Clear)) {
        pause_ms_window(window); 

        // It is assumed during a pause, the window
        // will not be rendered, however something else
        // will be in its place.
        return 1;
    }

    // Should return if any change requiring a re render has occured! 
    // Assume scan has already occured.
    
    const ms_difficulty *diff = DIFFS[window->game->diff_ind];

    // Screen coordinates of cursor. (in 16s)
    uint8_t screen_c_r = window->tmplt->s_r_offset + window->c_r;
    uint8_t screen_c_c = window->tmplt->s_c_offset + window->c_c;

    uint8_t game_c_r = screen_c_r + window->w_r;
    uint8_t game_c_c = screen_c_c + window->w_c;

    // Uncover logic.

    if (key_press(c_9)) {
        if (window->game->board[game_c_r][game_c_c].visibility != HIDDEN) {
            return 0; // Only can uncover hidden cells.
        }

        if (window->game->board[game_c_r][game_c_c].type == MINE) {
            // IF we hit a mine, do not uncover, simply end game.
            window->game->game_state = MS_DEFEAT;

            return 0; 
        }

        if (window->game->game_state == MS_WAITING) {
            start_ms_game(window->game, game_c_r, game_c_c);

            // Additionally, reset the timer.
            // NOTE probs will make this a separate function later.
            timer_Disable(1);
            timer_Set(1, ONE_SECOND);
            timer_SetReload(1, ONE_SECOND);
            timer_Enable(1, TIMER_32K, TIMER_0INT, TIMER_DOWN);
        } else {
            uncover_ms_cell(window->game, game_c_r, game_c_c);
        }

        // Check for win after uncover.
        if (window->game->non_exposed_cells == diff->mines) {
            window->game->game_state = MS_WIN;
        }

        // NOTE Slight optimizations could be put here.
        load_ms_window(window); // Reload screen.
                                
        return 1;
    }

    // Flagging Logic.

    if (key_press(c_7)) {
        if (window->game->board[game_c_r][game_c_c].visibility == HIDDEN &&
                window->game->flags_left > 0) {
            // Only allow a flag place if we have flags left.
            window->game->flags_left--;
            window->game->board[game_c_r][game_c_c].visibility = FLAGGED;
            window->render[screen_c_r][screen_c_c].actual_vc.fg = 0;

            return 1;
        }
        
        if (window->game->board[game_c_r][game_c_c].visibility == FLAGGED) {
            // Take back a flag here.
            window->game->flags_left++;
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
        if (game_c_r == diff->grid_height - 1) {
            return 0;
        }

        if (window->c_r == window->tmplt->s_height - 1) {
            window->w_r++;
            load_ms_window(window);
            return 1;
        }

        window->c_r++;
    } else if (key_press(c_6) || key_press(c_Right)) {
        if (game_c_c == diff->grid_width - 1) {
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
                            tiles16_tiles[BG_NO_RENDER + bv_cell->actual_vc.fg], x_p, y_p);
                }
            } 

            // Swap buffer and screen!
            bv_cell->buffer_vc = bv_cell->screen_vc;
            bv_cell->screen_vc = bv_cell->actual_vc;
        }
    } 
}

void reset_render_ms_window(ms_window *window) {
    uint8_t r, c;
    for (r = 0; r < window->tmplt->w_height; r++) {
        for (c = 0; c < window->tmplt->w_width; c++) {
            INIT_VIS_CELL(window->render[r][c].buffer_vc);
            INIT_VIS_CELL(window->render[r][c].screen_vc);
        }
    }
}

void del_ms_window(ms_window *window) {
    // NOTE, game no longer indie of window.
    del_ms_game(window->game);

    uint8_t r;
    for (r = 0; r < window->tmplt->w_height; r++) {
        safe_free(MS_WINDOW_CHANNEL, window->render[r]);
    } 

    safe_free(MS_WINDOW_CHANNEL, window->render);
    safe_free(MS_WINDOW_CHANNEL, window);

    // NOTE, the game is not freed here!!!!
}
