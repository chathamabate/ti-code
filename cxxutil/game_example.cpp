#include "game_example.h"
#include "cutil/keys.h"
#include "cxxutil/game.h"
#include <cstdint>
#include <graphx.h> 
#include <stdio.h>

using namespace cxxutil;

// Global State Code!

MoveCounter::MoveCounter() {
    this->counter = 0;
}

uint8_t MoveCounter::getCounter() {
    return this->counter;
}

void MoveCounter::setCounter(uint8_t c) {
    this->counter = c;
}

Request IntoStartScreen::run() {
    this->setNextGS(new StartScreen(this->getGlobalState())); 
    return C_REQ;
}

IntoStartScreen::IntoStartScreen(MoveCounter *mc) : TransitionState(mc) {}

constexpr uint8_t SS_KEYS_LEN = 2;
const c_key_t SS_KEYS[SS_KEYS_LEN] = {
    c_Clear, c_Enter
};

constexpr uint8_t EXIT_GAME = 0;
constexpr uint8_t TO_GAMEPLAY = 1;

Request StartScreen::enter() {
    set_focused_keys(SS_KEYS, SS_KEYS_LEN); 
    gfx_SetTextScale(1, 2);
    gfx_SetTextFGColor(0);
    return C_REQ; 
}

Request StartScreen::update() {
    scan_focused_keys();

    if (key_press(c_Enter)) {
        return {
            .type = RequestType::EXIT,
            .code = TO_GAMEPLAY
        };
    }

    if (key_press(c_Clear)) {
        return {
            .type = RequestType::EXIT,
            .code = EXIT_GAME
        };
    }

    return C_REQ;
}

void StartScreen::render() {
    if (!this->redraw) {
        return;
    }

    gfx_FillScreen(6);
    gfx_PrintStringXY("Press CLEAR to exit!", 16, 16);
    gfx_PrintStringXY("Press ENTER to play!", 16, 48);

    char buff[30];
    sprintf(buff, "Moves Made : %d", this->getGlobalState()->getCounter());
    gfx_PrintStringXY(buff, 16, 80);

    gfx_SwapDraw();
    this->redraw = false;
}

Request StartScreen::exit(uint8_t exit_code) {
    if (exit_code == EXIT_GAME) {
        // Exit stuff can go here...
        // Or in another transition state if wanted!

        return EX_REQ; 
    }

    if (exit_code == TO_GAMEPLAY) {
        // Create next transition!
        this->setNextTrans(new IntoPlayScreen(this->getGlobalState()));
        return C_REQ;
    }

    return F_REQ;
}

constexpr uint16_t SS_DEL = 100;
StartScreen::StartScreen(MoveCounter *mc) : GameState(SS_DEL, mc) {
    this->redraw = true;
}

// Play Screen code.

constexpr uint8_t PS_KEYS_LEN = 19;
const c_key_t PS_KEYS[PS_KEYS_LEN] = {
    c_8, c_4, c_5, c_6, c_Clear
};

Request PlayScreen::enter() {
    set_focused_keys(PS_KEYS, PS_KEYS_LEN);
    return C_REQ;
}

Request PlayScreen::update() {

}

void PlayScreen::render() {

}

Request PlayScreen::exit(uint8_t exit_code) {

}

const uint16_t PS_DEL = 100;
PlayScreen::PlayScreen(MoveCounter *mc) : GameState(PS_DEL, mc) {
    this->redraw = true;
}

    
