#include <graphx.h>
#include <cutil/misc.h>
#include <tice.h>

#include "states.h"

static void *enter_homepage(void *glb_state, void *loc_state);
static const loc_life_cycle *update_homepage(void *glb_state, void *loc_state);
static void render_homepage(void *glb_state, void *loc_state);
static void *exit_homepage(void *glb_state, void *loc_state);

const loc_life_cycle HOMEPAGE = {
    .enter = enter_homepage,
    .update = update_homepage,
    .render = render_homepage,
    .exit = exit_homepage
};

static void *enter_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    // Create state!!!
    return NULL;
}

static const loc_life_cycle *update_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    if (os_GetCSC() == sk_Clear) {
        return NULL; 
    }

    return &HOMEPAGE;
}

static int s = 0;

static void render_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    gfx_FillScreen((s = (s + 1) % 2) ? 20 : 50);

    // gfx_SwapDraw();
}

static void *exit_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}