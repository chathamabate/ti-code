
#include <cutil/gameloop.h>
#include <cutil/misc.h>
#include <cutil/cgraphx.h>

#include "states.h"

#include <tice.h>
#include <graphx.h>
#include <keypadc.h>

#include <cutil/menu.h>
#include <cutil/keys.h>
#include <cutil/data.h>

#include "gfx/ms_palette.h"
#include "gfx/tiles16.h"
#include "gfx/borders.h"
#include "ti/getcsc.h"

// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

static void *enter_game(void) {
    set_malloc_fail_routine(graphics_malloc_fail_routine);

    set_repeat_delay(10);
    srand(rtc_Time()); // Set random seed.

    gfx_Begin();
    gfx_SetDrawBuffer();
    gfx_SetPalette(ms_palette, sizeof_ms_palette, 0);
    gfx_SetTransparentColor(0);

    return NULL; // No Global state yet.
}

static void exit_game(void *glb_state) {
    (void)glb_state;

    gfx_End();

    set_malloc_fail_routine(normal_malloc_fail_routine);
}

const glb_life_cycle GLOBAL_LC = {
    .enter = enter_game,
    .exit = exit_game
};

int main(void) {
    run_game(50, &GLOBAL_LC, &HOMEPAGE);

    return 0;
}
