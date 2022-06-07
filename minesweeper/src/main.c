
#include <cutil/gameloop.h>
#include <cutil/misc.h>

#include "homepage.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

int main(void) {
    gfx_SetDrawBuffer();
    mf_routine = graphics_malloc_fail_routine;

    gfx_Begin();

    run_game(500, &HOMEPAGE);

    gfx_End();

    return 0;
}