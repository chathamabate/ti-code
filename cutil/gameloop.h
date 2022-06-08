#ifndef GAMELOOP_H
#define GAMELOOP_H

#include <stdint.h>

// A local enter function is called when a new life cycle is being entered.
// A life cycles should have one constant transition state format.
typedef void *(*loc_enter_func)(void *glb_state, void *trans_state);

// An update function is used to update the local and global state.
// It returns the local life cycle to use on the next iteration of the
// gameloop.
//
// To exit the game loop, an update function should return NULL.
typedef const struct local_life_cycle *(*loc_update_func)(void *glb_state, void *loc_state);

// A render function takes the global and local states.
// It renders the local scene.
typedef void (*loc_render_func)(void *glb_state, void *loc_state);

// A local exit function is called when there is a life cycle switch.
// It should clean up the local state and return the correct transition state into the next
// life cycle. (next_lc is next_lc to be run... NULL if exiting)
typedef void *(*loc_exit_func)(void *glb_state, void *loc_state, const struct local_life_cycle *next_loc_lc);

typedef struct local_life_cycle {
    // Enter will interpret the transition state.
    // Then construct the new state for this part of the game.
    // It will the clean up the transition state if it needs to.
    loc_enter_func enter;

    // Game loop methods for rendering and updating.
    loc_update_func update;
    loc_render_func render;

    // Exit will clean up the current state and return a transition state.
    // A transition state will contain necessary information for the next
    // state. 
    loc_exit_func exit;
} loc_life_cycle;

// This should do all needed set up for the game,
// and return the global state object.
typedef void *(*glb_enter_func)(void);

// This should clean up the global state,
// and execute all necessary exit procedures for the game.
typedef void (*glb_exit_func)(void *glb_state);

typedef struct {
    glb_enter_func enter;
    glb_exit_func exit;
} glb_life_cycle;

// Run a game.
void run_game(uint16_t delay_ms, const glb_life_cycle *gb_lc, const loc_life_cycle *init_loc_lc);

/*

// Template Local Life Cycle...

static void *enter_scene(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}

static const loc_life_cycle *update_scene(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;

    return NULL;
}

static void render_scene(void *glb_state, void *loc_state) {
    (void)glb_state;
    (void)loc_state;
}

static void *exit_homepage(void *glb_state, void *loc_state, const loc_life_cycle *next_loc_lc) {
    (void)glb_state;
    (void)loc_state;
    (void)next_loc_lc

    return NULL;
}

const loc_life_cycle SCENE = {
    .enter = enter_scene,
    .update = update_scene,
    .render = render_scene,
    .exit = exit_scene
};

*/

#endif