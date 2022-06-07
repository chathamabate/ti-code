#include "gameloop.h"
#include "stdlib.h"
#include <tice.h>

void run_game(uint16_t delay_ms, const  gs_life_cycle *init_lc) {
    const gs_life_cycle *curr_lc = init_lc;
    const gs_life_cycle *next_lc = NULL;

    void *state = NULL;

    do {
        state = curr_lc->enter(state);

        // This runs until a new life cycle is requested.
        while ((next_lc = curr_lc->update(state)) == curr_lc) {
            curr_lc->render(state);
            delay(delay_ms);
        }

        // When a new life cycle is requested, the current one is
        // exited.
        state = curr_lc->exit(state);

        // then, the current life cycle is set to the next life cycle.
    } while ((curr_lc = next_lc));
}