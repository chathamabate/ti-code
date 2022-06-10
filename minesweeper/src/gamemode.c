
#include "states.h"

#include <stdlib.h>

// Game mode code...


// Template Local Life Cycle...

static void *enter_gamemode(void *glb_state, void *trans_state) {
    (void)glb_state;

    // Gamemode page gets a NULL transiiton state.
    (void)trans_state;

    return NULL;
}

static const loc_life_cycle *update_gamemode(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}

static void render_gamemode(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_gamemode(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc;

    return NULL;
}

const loc_life_cycle GAMEMODE = {
    .enter = enter_gamemode,
    .update = update_gamemode,
    .render = render_gamemode,
    .exit = exit_gamemode
};