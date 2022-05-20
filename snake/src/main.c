
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

    void *s; // state, can be anything!
} game_state;

// The state of the game.
static uint8_t curr_state = START_PAGE;
static uint8_t next_state = START_PAGE;

// Whether or not a redraw is needed.
static uint8_t redraw_needed = 1;

static button main_menu_bts[3];
static menu main_menu;

static button defeat_menu_bts[2];
static menu defeat_menu;

static button highscores_menu_bts[1];
static menu highscores_menu;

static hs_entry sb[SB_SIZE];

// Globals for entering highscore name!
static char sb_name_buff[SB_NAME_LEN + 1];
static uint8_t sb_name_ind = 0;

static snake_game *sg = NULL;

// Last score to be scored, and where to place it in the 
// scoreboard.
static uint16_t last_score = 0;
static uint8_t placement = SB_SIZE;
static hs_entry new_entry;

// Whether or not a new highscore has been achieved.
static uint8_t archive_needed = 0;

// Game state array.
static game_state gsa[EXIT + 1];

// Macro for initing a game state.
#define store_game_state(gsi, n_enter, n_render, n_update, n_exit, n_s) \
    gsa[gsi].enter = n_enter; \
    gsa[gsi].render = n_render; \
    gsa[gsi].update = n_update; \
    gsa[gsi].exit = n_exit; \
    gsa[gsi].s = n_s

#define retr_menu(gsi) ((menu *)(gsa[gsi].s))

// Constant macros for drawing scores.
#define SCORE_TEXT_W 2
#define SCORE_TEXT_H 2

#define START_PAGE_MSG "Snake"
#define START_PAGE_TXT_W 4
#define START_PAGE_TXT_H 5
static uint8_t START_PAGE_TITLE_W;

#define DEFEAT_PAGE_MSG "Defeat"
static uint8_t DEFEAT_PAGE_TITLE_W;

#define NEW_HS_PAGE_MSG "Highscore!"
static uint8_t NEW_HS_PAGE_TITLE_W;

// Initialize above globals.
static void init_globals(void);

// All enter functions.
static void enter_noop(void);
static void enter_in_play(void);
static void enter_new_highscore(void);

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
static void exit_new_highscore(void);
static void exit_noop(void);

static void init_globals(void) {
    srand(rtc_Time());

    main_menu_bts[0].text = "Play";   
    main_menu_bts[0].link = IN_PLAY;

    main_menu_bts[1].text = "Highscores";
    main_menu_bts[1].link = HIGHSCORES;

    main_menu_bts[2].text = "Exit";
    main_menu_bts[2].link = EXIT;

    install_menu(&main_menu, main_menu_bts, 3);

    defeat_menu_bts[0].text = "Play Again";
    defeat_menu_bts[0].link = IN_PLAY;

    defeat_menu_bts[1].text = "Main Menu";
    defeat_menu_bts[1].link = START_PAGE;

    install_menu(&defeat_menu, defeat_menu_bts, 2);

    highscores_menu_bts[0].text = "Main Menu";
    highscores_menu_bts[0].link = START_PAGE;

    install_menu(&highscores_menu, highscores_menu_bts, 1);

    // Store all games states in the gsa.
    store_game_state(START_PAGE, enter_noop, render_start_page, update_menus, exit_noop, &main_menu);
    store_game_state(HIGHSCORES, enter_noop, render_highscores, update_menus, exit_noop, &highscores_menu);
    store_game_state(IN_PLAY, enter_in_play, render_in_play, update_in_play, exit_in_play, NULL);
    store_game_state(DEFEAT, enter_noop, render_defeat, update_menus, exit_noop, &defeat_menu);
    store_game_state(NEW_HIGHSCORE, enter_new_highscore, render_new_highscore, 
        update_new_highscore, exit_new_highscore, NULL);
    store_game_state(EXIT, enter_noop, render_noop, update_noop, exit_noop, NULL);

    // Could probs be better in next project tbh...
    gfx_SetTextScale(1, 1);
    START_PAGE_TITLE_W = gfx_GetStringWidth(START_PAGE_MSG);
    DEFEAT_PAGE_TITLE_W = gfx_GetStringWidth(DEFEAT_PAGE_MSG);
    NEW_HS_PAGE_TITLE_W = gfx_GetStringWidth(NEW_HS_PAGE_MSG);
}

int main(void) {
    os_ClrHome();

    if (!load_sb(sb)) {
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
        SB_ERROR(); // archive error!
    }

    return 0;
}

//
// Enter Functions ----------------------------------------------------------------
//

static void enter_noop(void) {
    redraw_needed = 1;
}

static void enter_in_play(void) {
    redraw_needed = 1;
    sg = new_snake_game(8);
}

static void enter_new_highscore(void) {
    redraw_needed = 1;

    uint8_t i;
    for (i = 0; i < SB_NAME_LEN; i++) {
        sb_name_buff[i] = 'A';
    }  
} 

//
// Render Functions ----------------------------------------------------------------
//

static void render_noop(void) {

}

static void render_start_page(void) {
    gfx_FillScreen(COLOR_5);

    // gfx_SetMonospaceFont(8);
    gfx_SetTextScale(START_PAGE_TXT_W, START_PAGE_TXT_H);

    gfx_SetTextFGColor(COLOR_1);
    gfx_PrintStringXY(START_PAGE_MSG, center(START_PAGE_TITLE_W * START_PAGE_TXT_W), 45);

    render_menu_xy(&main_menu, center(main_menu.width), 120);
}

static void render_highscores(void) {
    gfx_FillScreen(COLOR_5);
    render_sb_xy(sb, center(SB_WIDTH), 30);
    render_menu_xy(&highscores_menu, center(highscores_menu.width), 30 + SB_HEIGHT + 30);
}

static void render_in_play(void) {
    render_snake_game(sg);
}

static void render_last_score(uint8_t y) {
    char score_buff[14]; 
    sprintf(score_buff, "Score: %d", last_score); 

    gfx_SetTextScale(SCORE_TEXT_W, SCORE_TEXT_H); 
    uint16_t score_width = gfx_GetStringWidth(score_buff);

    gfx_PrintStringXY(score_buff, center(score_width), y);
}

static void render_defeat(void) {
    gfx_FillScreen(COLOR_4);

    uint8_t y = 30;
    gfx_SetTextScale(START_PAGE_TXT_W, START_PAGE_TXT_H);
    gfx_SetTextFGColor(COLOR_2);
    gfx_PrintStringXY(DEFEAT_PAGE_MSG, center(START_PAGE_TXT_W * DEFEAT_PAGE_TITLE_W), y);

    y += (START_PAGE_TXT_H * 8) + 20;

    render_last_score(y);

    y += (SCORE_TEXT_H * 8) + 20;

    render_menu_xy(&defeat_menu, center(defeat_menu.width), y);
}

// Text size for the menu used for entering names.
#define NEW_HS_TXT_W 2 
#define NEW_HS_TXT_H 2

#define NEW_HS_DIAL_BORDER 5 // Border is double sided for the name dial.

// Works since all upper case letters are monospace.
#define NEW_HS_DIAL_CELL_W ((2 * NEW_HS_DIAL_BORDER) + (8 * NEW_HS_TXT_W))

#define NEW_HS_DIAL_W (SB_NAME_LEN * NEW_HS_DIAL_CELL_W)
#define NEW_HS_DIAL_H ((8 * NEW_HS_TXT_H) + (2 * NEW_HS_DIAL_BORDER))

#define NEW_HS_DIAL_FG COLOR_0
#define NEW_HS_DIAL_BG COLOR_3
#define NEW_HS_DIAL_S_BG COLOR_4
#define NEW_HS_DIAL_S_FG COLOR_0

static void render_new_highscore(void) {
    gfx_FillScreen(COLOR_5);

    uint8_t y = 30;
    gfx_SetTextScale(START_PAGE_TXT_W, START_PAGE_TXT_H);

    gfx_SetTextFGColor(COLOR_1);
    gfx_PrintStringXY(NEW_HS_PAGE_MSG, center(START_PAGE_TXT_W * NEW_HS_PAGE_TITLE_W), y);

    y += (START_PAGE_TXT_H * 8) + 20;
    render_last_score(y);

    y += (SCORE_TEXT_H * 8) + 20;

    uint8_t x = center(NEW_HS_DIAL_W);

    gfx_SetColor(NEW_HS_DIAL_BG);
    gfx_FillRectangle(x, y, NEW_HS_DIAL_W, NEW_HS_DIAL_H);

    gfx_SetTextScale(NEW_HS_TXT_W, NEW_HS_TXT_H);

    gfx_SetColor(NEW_HS_DIAL_S_BG); // Highlighted Cell BG.
    gfx_SetTextFGColor(NEW_HS_DIAL_FG); // Normal Cell FG.

    char letter_buff[2];
    letter_buff[1] = '\0';

    uint8_t i;
    for (i = 0; i < SB_NAME_LEN; i++, x += NEW_HS_DIAL_CELL_W) {
        letter_buff[0] = sb_name_buff[i];

        if (i == sb_name_ind) {
            gfx_FillRectangle(x, y, NEW_HS_DIAL_CELL_W, NEW_HS_DIAL_H);
            gfx_SetTextFGColor(NEW_HS_DIAL_S_FG);
            gfx_PrintStringXY(letter_buff, x + NEW_HS_DIAL_BORDER, y + NEW_HS_DIAL_BORDER);
            gfx_SetTextFGColor(NEW_HS_DIAL_FG); // Revert back to normal FG color.

            continue;
        } 
        
        // Normal case, just draw the text.
        gfx_PrintStringXY(letter_buff, x + NEW_HS_DIAL_BORDER, y + NEW_HS_DIAL_BORDER);            
    }
}

//
// Upate Functions ----------------------------------------------------------------
//

static void update_noop(void) {

}

static void update_menus(void) {
    sk_key_t key = os_GetCSC();

    switch (key) {
    case sk_Up:
    case sk_8:
        menu_up(retr_menu(curr_state)); // Should never be NULL.
        redraw_needed = 1;
        break;
    case sk_Down:
    case sk_5:
        menu_down(retr_menu(curr_state));
        redraw_needed = 1;
        break;
    case sk_Enter:
        next_state = menu_link(retr_menu(curr_state));
        break;
    }
}

static void update_in_play(void) {
    sk_key_t key = os_GetCSC(); // Consider using is down in future!

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
        next_state = placement < SB_SIZE ? NEW_HIGHSCORE : DEFEAT;
    }
}

static void update_new_highscore(void) {
    char curr = sb_name_buff[sb_name_ind];

    switch (os_GetCSC()) {
    case sk_Up:
    case sk_8:
        sb_name_buff[sb_name_ind] = curr == 'A' ? 'Z' : curr - 1;
        break;
    case sk_Down:
    case sk_5:
        sb_name_buff[sb_name_ind] = curr == 'Z' ? 'A' : curr + 1;
        break;
    case sk_Left:
    case sk_4:
        sb_name_ind = sb_name_ind ? sb_name_ind - 1 : SB_NAME_LEN - 1;
        break;
    case sk_Right:
    case sk_6:
        sb_name_ind = (sb_name_ind + 1) % SB_NAME_LEN;
        break;
    case sk_Enter:
        next_state = HIGHSCORES;
        return;
    default:
        return;
    }

    redraw_needed = 1;
}

//
// Exit Functions ----------------------------------------------------------------
//

static void exit_noop(void) {
    // Do Nothing.
}

static void exit_in_play(void) {
    destroy_snake_game(sg);
}

static void exit_new_highscore(void) {
    new_entry.score = last_score;
    strcpy(new_entry.name, sb_name_buff);

    sb_insert(sb, placement, &new_entry);

    archive_needed = 1;
}