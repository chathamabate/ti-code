#ifndef STATES_H
#define STATES_H

#include <cutil/gameloop.h>

// Global state for the entire game.
typedef struct {

} ms_glb_state;

extern const loc_life_cycle HOMEPAGE;
// Homepage has no transition state... 
// should just be null.


extern const loc_life_cycle GAMEMODE;
// Gamemode has no transition state...
// should just be null.

extern const loc_life_cycle GAMEPLAY;
// Tranisition state into GAMEPLAY
// should be a uint8_t *.
// This should point to the difficulty index to play with.
// The memory used by the pointer should be dynamic
// and will be freed upon entering gameplay.

extern const loc_life_cycle HIGHSCORES;
// Homepage has no transition state... 
// should just be null.

extern const loc_life_cycle VICTORY;

typedef struct {
    uint16_t score;     // Score from the game.
    uint8_t diff_ind;   // Difficulty index of the game played.
} trans_victory;

extern const loc_life_cycle DEFEAT;
// Transition state should be a uint8_t *.
// This should be the difficulty index of the game lost.

extern const loc_life_cycle INSTRUCTIONS;
// No transition state... just NULL.

#endif
