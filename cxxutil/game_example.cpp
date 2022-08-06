#include "game_example.h"
#include "cutil/keys.h"
#include "cxxutil/game.h"
#include <graphx.h> 
#include <stdio.h>

using namespace cxxutil;

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
    gfx_FillScreen(6);
    gfx_PrintStringXY("Press CLEAR to exit!", 16, 16);
    gfx_PrintStringXY("Press ENTER to play!", 16, 48);

    char buff[30];
    sprintf(buff, "Moves Made : %d", this->getGlobalState()->getCounter());
    gfx_PrintStringXY(buff, 16, 80);

    gfx_SwapDraw();
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


    
