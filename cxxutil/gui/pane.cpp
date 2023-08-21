# include "./pane.h"

#include "cxxutil/core/mem.h"
#include "ti/screen.h"
#include <graphx.h>

using namespace cxxutil::gui;


void cxxutil::gui::renderClippedText(uint24_t x, uint8_t y,
        const char *text, uint24_t clipWidth) {
    const char *iter = text;
    uint24_t relX = 0;

    char currChar = *text;
    uint8_t currCharWidth = gfx_GetCharWidth(currChar);

    gfx_SetTextXY(x, y);
    
    while (currChar != '\0' && relX + currCharWidth <= clipWidth) {
        gfx_PrintChar(currChar); 
        relX += currCharWidth;

        currChar = *(++iter);
        currCharWidth = gfx_GetCharWidth(currChar);
    } 
}

Pane::Pane(uint8_t memChnl) : core::SafeObject(memChnl) {
    this->inFocus = false;
}

Pane::Pane() : Pane(core::CXX_DEF_CHNL) {
}

Pane::~Pane() {
}
