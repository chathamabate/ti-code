#include <graphx.h>
#include <cutil/misc.h>
#include <tice.h>

#include "states.h"

static void *enter_homepage(void *transition_state);
static const gs_life_cycle *update_homepage(void *state);
static void render_homepage(void *state);
static void *exit_homepage(void *state);

const gs_life_cycle HOMEPAGE = {
    .enter = enter_homepage,
    .update = update_homepage,
    .render = render_homepage,
    .exit = exit_homepage
};

static void *enter_homepage(void *transition_state) {
    (void)transition_state;

    // Create state!!!
    return NULL;
}

static const gs_life_cycle *update_homepage(void *state) {
    (void)state;

    if (os_GetCSC() == sk_Clear) {
        return NULL; 
    }

    return &HOMEPAGE;
}

static int s = 0;

static void render_homepage(void *state) {
    (void)state;

    gfx_FillScreen((s = (s + 1) % 2) ? 20 : 50);

    // gfx_SwapDraw();
}

static void *exit_homepage(void *state) {
    (void)state;

    return NULL;
}