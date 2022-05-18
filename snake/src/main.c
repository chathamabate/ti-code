
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <tice.h>
#include <string.h>

#include "misc.h"
#include "snake.h"
#include "scores.h"

#define DELAY 50

#define START_PAGE      0
#define HIGHSCORES      1
#define IN_PLAY         2
#define DEFEAT          3
#define NEW_HIGHSCORE   4
#define EXIT            5 
// For sizing, make sure exit is always the last state.

#define PAUSE(msg) \
    os_PutStrFull(msg); \
    os_NewLine(); \
    while (os_GetCSC() != sk_Clear)
    
#define SB_ERROR() \
    os_ClrHome(); \
    os_PutStrFull(sb_error_msg); \
    while (os_GetCSC() != sk_Clear); \
    return 0

// Simple typedef for runnable actions.
typedef  void (*action)(void);

typedef struct {
    action enter;
    action render;
    action update;
    action exit;

    menu *m; // Not always needed.
} game_state;

// The state of the game.
static uint8_t curr_state = START_PAGE;
static uint8_t next_state = START_PAGE;

// Whether or not a redraw is needed.
static uint8_t redraw_needed = 1;

static button main_menu_bts[3];
static menu *main_menu;

static button defeat_menu_bts[2];
static menu *defeat_menu;

static button highscores_menu_bts[1];
static menu *highscores_menu;

static hs_entry *sb = NULL;
static snake_game *sg = NULL;

// Last score to be scored, and where to place it in the 
// scoreboard.
static uint16_t last_score = 0;
static uint8_t placement = SB_SIZE;

// Whether or not a new highscore has been achieved.
static uint8_t archive_needed = 0;

// Game state array.
static game_state gsa[EXIT + 1];

// Macro for initing a game state.
#define store_game_state(gsi, n_enter, n_render, n_update, n_exit, n_m) \
    gsa[gsi].enter = n_enter; \
    gsa[gsi].render = n_render; \
    gsa[gsi].update = n_update; \
    gsa[gsi].exit = n_exit; \
    gsa[gsi].m = n_m

// Initialize above globals.
static void init_globals(void);

// Clean up above globals.
static void cleanup_globals(void);

// All enter functions.
static void enter_noop(void);
static void enter_in_play(void);

// All scene render functions.
static void render_start_page(void);
static void render_highscores(void);
static void render_in_play(void);
static void render_defeat(void);
static void render_new_highscore(void);
static void render_noop(void); 

// update_menus is usable for
// START_PAGE, HIGH_SCORES, and DEFEAT

// All update functions.
static void update_menus(void);
static void update_in_play(void);
static void update_new_highscore(void);
static void update_noop(void);

// all exit functions.
static void exit_in_play(void);
static void exit_noop(void);

static void init_globals(void) {
    main_menu_bts[0].text = "Play";   
    main_menu_bts[0].link = IN_PLAY;

    main_menu_bts[1].text = "Highscores";
    main_menu_bts[1].link = HIGHSCORES;

    main_menu_bts[2].text = "Exit";
    main_menu_bts[2].link = EXIT;

    main_menu = new_menu(main_menu_bts, 3);

    defeat_menu_bts[0].text = "Play Again";
    defeat_menu_bts[0].link = IN_PLAY;

    defeat_menu_bts[1].text = "Main Menu";
    defeat_menu_bts[1].link = START_PAGE;

    defeat_menu = new_menu(defeat_menu_bts, 2);

    highscores_menu_bts[0].text = "Main Menu";
    highscores_menu_bts[0].link = START_PAGE;

    highscores_menu = new_menu(highscores_menu_bts, 1);

    // Store all games states in the gsa.
    store_game_state(START_PAGE, enter_noop, render_start_page, update_menus, exit_noop, main_menu);
    store_game_state(HIGHSCORES, enter_noop, render_highscores, update_menus, exit_noop, highscores_menu);
    store_game_state(IN_PLAY, enter_in_play, render_in_play, update_in_play, exit_in_play, NULL);
    store_game_state(DEFEAT, enter_noop, render_defeat, update_menus, exit_noop, NULL);
    store_game_state(NEW_HIGHSCORE, enter_noop, render_new_highscore, update_new_highscore, exit_noop, NULL);
    store_game_state(EXIT, enter_noop, render_noop, update_noop, exit_noop, NULL);
}

static void cleanup_globals(void) {
    // Free all dynamic memory globals.
    free(main_menu);
    free(defeat_menu);
    free(highscores_menu);
    free(sb);
}

int main(void) {
    os_ClrHome();

    sb = load_sb();
    if (sb == NULL) {
        // Don't do anything if there's a load error.
        SB_ERROR();
    }

    init_globals();

    gfx_SetDrawBuffer();
    gfx_Begin();

    sg = new_snake_game(8);

    // Simple game loop.
    while (curr_state != EXIT) {
        if (redraw_needed) {
            gfx_FillScreen(255);
            gsa[curr_state].render();
            gfx_SwapDraw();

            redraw_needed = 0;
        }

        gsa[curr_state].update();

        if (curr_state != next_state) {
            gsa[curr_state].exit();
            gsa[next_state].enter();

            curr_state = next_state;
        }

        delay(DELAY);
    }

    gfx_End();

    if (archive_needed && !archive_sb(sb)) {
        cleanup_globals();
        SB_ERROR(); // archive error!
    }

    cleanup_globals();

    return 0;
}

#define START_PAGE_TXT_W 4
#define START_PAGE_TXT_H 5

// Title says Snake. (5 characters)
#define START_PAGE_TITLE_W (5 * 8 * START_PAGE_TXT_W)

static void render_noop(void) {

}

static void render_start_page(void) {
    gfx_FillScreen(COLOR_5);

    gfx_SetMonospaceFont(8);
    gfx_SetTextScale(START_PAGE_TXT_W, START_PAGE_TXT_H);

    gfx_SetTextFGColor(COLOR_1);
    gfx_PrintStringXY("Snake", center(START_PAGE_TITLE_W), 45);

    render_menu_xy(main_menu, center(main_menu->width), 120);
}

static void render_highscores(void) {

}

static void render_in_play(void) {
    render_snake_game(sg);
}

static void render_defeat(void) {

}

static void render_new_highscore(void) {

}

static void enter_noop(void) {
    redraw_needed = 1;
}

#define CELL_SIZE 8

static void enter_in_play(void) {
    redraw_needed = 1;
    sg = new_snake_game(CELL_SIZE);
}

static void update_noop(void) {

}

static void update_menus(void) {
    sk_key_t key = os_GetCSC();

    switch (key) {
    case sk_Up:
    case sk_8:
        menu_up(gsa[curr_state].m); // Should never be NULL.
        redraw_needed = 1;
        break;
    case sk_Down:
    case sk_5:
        menu_down(gsa[curr_state].m);
        redraw_needed = 1;
        break;
    case sk_Enter:
        next_state = menu_link(gsa[curr_state].m);
        break;
    }
}

static void update_in_play(void) {
    sk_key_t key = os_GetCSC();

    switch (key) {
    case sk_Up:
    case sk_8:
        sg->direction = NORTH;
        break;
    case sk_Down:
    case sk_5:
        sg->direction = SOUTH;
        break;
    case sk_Left:
    case sk_4:
        sg->direction = WEST;
        break;
    case sk_Right:
    case sk_6:
        sg->direction = EAST;
        break;
    }

    if (sg->first->direction == STILL) {
            sg->first->direction = sg->direction;
    } else if ((sg->first->size > 1 || sg->first->prev) && is_opposite(sg->direction, sg->first->direction)) {
        // Cannot pick opposite direction when snake has size greater than 1.
        sg->direction = sg->first->direction;
    } 

    grow(sg);

    if (sg->in_play) {
        shrink(sg);
        redraw_needed = 1;
    } else {
        last_score = sg->score;
        placement = sb_placement(sb, last_score);
        next_state = START_PAGE;
        // next_state = placement < SB_SIZE ? NEW_HIGHSCORE : DEFEAT;
    }
}

static void update_new_highscore(void) {

}

static void exit_noop(void) {
    // Do Nothing.
}

static void exit_in_play(void) {
    destroy_snake_game(sg);
}
