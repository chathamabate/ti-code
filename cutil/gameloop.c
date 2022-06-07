#include "gameloop.h"
#include "stdlib.h"
#include <tice.h>

void run_game(uint16_t delay_ms, const glb_life_cycle *glb_lc, const loc_life_cycle *init_loc_lc) {
    const loc_life_cycle *curr_loc_lc = init_loc_lc;
    const loc_life_cycle *next_loc_lc = NULL;

    // global and local state pointers.
    void *glb_state = glb_lc->enter();
    void *loc_state = NULL;

    do {
        loc_state = curr_loc_lc->enter(glb_state, loc_state);

        // This runs until a new life cycle is requested.
        while ((next_loc_lc = curr_loc_lc->update(glb_state, loc_state)) == curr_loc_lc) {
            curr_loc_lc->render(glb_state, loc_state);
            delay(delay_ms);
        }

        // When a new life cycle is requested, the current one is
        // exited.
        loc_state = curr_loc_lc->exit(glb_state, loc_state);

        // then, the current life cycle is set to the next life cycle.
    } while ((curr_loc_lc = next_loc_lc));

    glb_lc->exit(glb_state);
}