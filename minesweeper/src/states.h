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
// should be a const difficulty * 
// describing which difficulty to play.



#endif
