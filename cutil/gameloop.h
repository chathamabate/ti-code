#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <stdint.h>

// A state switch func, takes a pointer to the current state.
// It then creates a new state to switch to and returns that state.
// If the current state needs to be freed, this should be done in this
// function.
typedef void *(*gs_switch_func)(void *);

// An update function is used to update the current state.
// It returns the life cycle to use for the next iteration.
// To request game exit, NULL should be returned.
typedef const gs_life_cycle *(*gs_update_func)(void *);

// An state stay func will read from or write to the current state,
// then return.
typedef void (*gs_render_func)(void *);

typedef struct {
    // Enter will interpret the transition state.
    // Then construct the new state for this part of the game.
    // It will the clean up the transition state if it needs to.
    gs_switch_func enter;

    // Game loop methods for rendering and updating.
    gs_update_func update;
    gs_render_func render;

    // Exit will clean up the current state and return a transition state.
    // A transition state will contain necessary information for the next
    // state. 
    gs_switch_func exit;
} gs_life_cycle;

// Run a game.
void run_game(const gs_life_cycle *init_lc);

#endif