#include "gameloop.h"

void run_game(gs *game) {
    game->state = game->life_cycle->enter(game->state);
    
    while (1) {

    }
}