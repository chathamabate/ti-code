
#include <cutil/gameloop.h>
#include <cutil/misc.h>

#include "states.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

static void *enter_game(void) {
    gfx_SetDrawBuffer();
    mf_routine = graphics_malloc_fail_routine;
    gfx_Begin();

    return NULL;
}

static void exit_game(void *glb_state) {
    (void)glb_state;

    gfx_End();
    mf_routine = normal_malloc_fail_routine;
}

const glb_life_cycle GLOBAL_LC = {
    .enter = enter_game,
    .exit = exit_game
};

int main(void) {
    run_game(500, &GLOBAL_LC, &HOMEPAGE);

    return 0;
}