
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
#include "snake_render.h"

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

// Menus for each state.
static menu *menus[EXIT + 1];

static hs_entry *sb = NULL;
static snake_game *sg = NULL;

// Last score to be scored, and where to place it in the 
// scoreboard.
static uint16_t last_score = 0;
static uint8_t placement = SB_SIZE;

// Whether or not a new highscore has been achieved.
static uint8_t archive_needed = 0;

// These are the render and update functions for each game
// state.
static action render_funcs[EXIT + 1];
static action update_funcs[EXIT + 1];
static action enter_funcs[EXIT + 1];
static action exit_funcs[EXIT + 1];

// Initialize above globals.
void init_globals(void);

// Clean up above globals.
void cleanup_globals(void);

// All scene render functions.
void render_start_page(void);
void render_highscores(void);
void render_in_play(void);
void render_defeat(void);
void render_new_highscore(void);
void render_exit(void); // Should never be called.

// All enter functions.
void enter_noop(void);
void enter_in_play(void);

// This update function is usable for
// START_PAGE, HIGH_SCORES, and DEFEAT

// All update functions.
void update_menus(void);
void update_in_play(void);
void update_new_highscore(void);
void update_exit(void); // Should never be called.

// all exit functions.
void exit_noop(void);
void exit_in_play(void);

void init_globals(void) {
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

    menus[START_PAGE] = main_menu;
    menus[HIGHSCORES] = highscores_menu;
    menus[IN_PLAY] = NULL;
    menus[DEFEAT] = defeat_menu;
    menus[NEW_HIGHSCORE] = NULL;
    menus[EXIT] = NULL;

    render_funcs[START_PAGE] = render_start_page;
    render_funcs[HIGHSCORES] = render_highscores;
    render_funcs[IN_PLAY] = render_in_play;
    render_funcs[DEFEAT] = render_defeat;
    render_funcs[NEW_HIGHSCORE] = render_new_highscore;
    render_funcs[EXIT] = render_exit;

    update_funcs[START_PAGE] = update_menus;
    update_funcs[HIGHSCORES] = update_menus;
    update_funcs[IN_PLAY] = update_in_play;
    update_funcs[DEFEAT] = update_menus;
    update_funcs[NEW_HIGHSCORE] = update_new_highscore;
    update_funcs[EXIT] = update_exit;

    uint8_t state;
    for (state = 0; state < EXIT + 1; state++) {
        enter_funcs[state] = enter_noop;
        exit_funcs[state] = exit_noop;
    }

    enter_funcs[IN_PLAY] = enter_in_play;
    exit_funcs[IN_PLAY] = exit_in_play;
}

void cleanup_globals(void) {
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
            (render_funcs[curr_state])();
            gfx_SwapDraw();

            redraw_needed = 0;
        }

        (update_funcs[curr_state])();

        if (curr_state != next_state) {
            (exit_funcs[curr_state])();
            (enter_funcs[next_state])();

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

#define START_PAGE_BG_COLOR 247
#define START_PAGE_FG_COLOR 16

#define START_PAGE_TXT_W 4
#define START_PAGE_TXT_H 5

// Title says Snake. (5 characters)
#define START_PAGE_TITLE_W (5 * 8 * START_PAGE_TXT_W)

void render_start_page(void) {
    gfx_FillScreen(START_PAGE_BG_COLOR);

    gfx_SetMonospaceFont(8);
    gfx_SetTextScale(START_PAGE_TXT_W, START_PAGE_TXT_H);

    gfx_SetTextFGColor(START_PAGE_FG_COLOR);
    gfx_PrintStringXY("Snake", center(START_PAGE_TITLE_W), 45);

    render_menu_xy(main_menu, center(main_menu->width), 120);
}

void render_highscores(void) {

}

void render_in_play(void) {
    render_snake_game(sg);
}

void render_defeat(void) {

}

void render_new_highscore(void) {

}

void render_exit(void) {

}

void enter_noop(void) {
    redraw_needed = 1;
}

#define CELL_SIZE 8

void enter_in_play(void) {
    redraw_needed = 1;
    sg = new_snake_game(CELL_SIZE);
}

void update_menus(void) {
    sk_key_t key = os_GetCSC();

    switch (key) {
    case sk_Up:
    case sk_8:
        menu_up(menus[curr_state]);
        redraw_needed = 1;
        break;
    case sk_Down:
    case sk_5:
        menu_down(menus[curr_state]);
        redraw_needed = 1;
        break;
    case sk_Enter:
        next_state = menu_link(menus[curr_state]);
        break;
    }
}

void update_in_play(void) {
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

void update_new_highscore(void) {

}

void update_exit(void) {

}

void exit_noop(void) {
    // Do Nothing.
}

void exit_in_play(void) {
    destroy_snake_game(sg);
}
