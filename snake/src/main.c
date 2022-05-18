
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

#define START_PAGE      0
#define HIGHSCORES      1
#define IN_PLAY         2
#define DEFEAT          3
#define NEW_HIGHSCORE   4
#define EXIT            5

#define SB_ERROR() \
    os_ClrHome(); \
    os_PutStrFull(sb_error_msg); \
    while (os_GetCSC() != sk_Clear); \
    return 0

static uint8_t curr_state = START_PAGE;

static button main_menu_bts[3];
static menu *main_menu;

static button defeat_menu_bts[2];
static menu *defeat_menu;

static menu *selected_menu = NULL;
static hs_entry *sb = NULL;

// Initialize above globals.
void init_globals(void);

// Clean up above globals.
void cleanup_globals(void);

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
}

int main(void) {
    sb = load_sb();
    if (sb == NULL) {
        // Don't do anything if there's a load error.
        SB_ERROR();
    }

    init_globals();

    gfx_SetDrawBuffer();
    gfx_Begin();

    while (curr_state != EXIT) {
        gfx_FillScreen(255);
        sk_key_t key = os_GetCSC();

        if (curr_state == IN_PLAY) {

            continue;
        }

        if (curr_state == NEW_HIGHSCORE) {

            continue;
        }

        // Default case...
        // just normal menu manipulation.

        if (key == sk_Up) {
            
        }

    }

    gfx_FillScreen(255);

    render_sb(sb, c);

    gfx_SwapDraw();
    gfx_Wait();

    while (os_GetCSC() != sk_Clear);

    gfx_End();
    free(sb);
    return 0;
}