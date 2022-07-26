#include "game.h"

using namespace cxxutil;

template<typename T>
Game<T>::Game() { }

template<typename T>
Game<T>::~Game() {
    if (this->globalState) {
        delete this->globalState;
    }

    if (this->gameState) {
        delete this->gameState;
    }

    if (this->transState) {
        delete this->transState;
    }
}

template<typename T>
T *Game<T>::getGlobalState() {
    return this->globalState;
}

template<typename T>
StatusInfo Game<T>::run() {
    StatusInfo si;

    si = this->initGlobalState();
    if (si.status != Status::CONTINUE) {
        return si;
    }

    si = this->initTransState();
    if (si.status != Status::CONTINUE) {
        return si;
    }

    // If we make it here, we know our transition is
    // not an exit transition... we must be able to 
    // create a gamestate with it.
    
    while (this->transState->getTransInfo().status == Status::CONTINUE) {
        // Create next game state.
        this->gameState = this->transState->transition();

        // Remove transition state from memory.
        delete this->transState;
        this->transState = nullptr;

    }
    
    
       
}
