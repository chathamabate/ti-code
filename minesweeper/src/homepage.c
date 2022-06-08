#include <graphx.h>
#include <cutil/misc.h>
#include <tice.h>

#include "states.h"

// Main menu button labels.
const char *button_labels[4] = {
    "Play",
    "Highscores",
    "Instructions",
    "Exit"
};

// The local state for the homepage.
typedef struct {
    uint8_t selection;
} homepage_state;

static void *enter_homepage(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

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

const loc_life_cycle HOMEPAGE = {
    .enter = enter_homepage,
    .update = update_homepage,
    .render = render_homepage,
    .exit = exit_homepage
};