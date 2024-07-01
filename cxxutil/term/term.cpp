
#include "./term.h"
#include "cxxutil/core/mem.h"
#include <graphx.h>

using namespace cxxutil::term;

const cell_state_t cxxutil::term::DEF_CELL_STATE = {
    .style = {
        .fgColor = ANSII_STRONG_WHITE,
        .bgColor = ANSII_STRONG_BLACK,
        .underline = 0,
        .slowBlink = 0,
        .fastBlink = 0,
        .strikeThru = 0,
    },
    .character = ' '
};

const uint8_t cxxutil::term::DEF_COLOR_MAP[16] = {
    33, 
    200,
    5, 
    230,
    27,
    217,
    159,
    247,

    0,
    224,
    7,
    231,
    24,
    248,
    31,
    255,
};

Terminal::Terminal(const terminal_config_t &cfg) 
    : Terminal(core::CXX_DEF_CHNL, cfg) {
}

Terminal::Terminal(uint8_t chnl, const terminal_config_t &cfg) 
    : core::SafeObject(chnl), config(cfg) {
    const size_t numCells = this->config.rows * this->config.cols;

    this->renderStateVisible = 0;
    this->renderState = new core::SafeArray<cell_state_t>(chnl, numCells);
    this->currState = new core::SafeArray<cell_state_t>(chnl, numCells);

    for (size_t i = 0; i < numCells; i++) {
        this->renderState->set(i, DEF_CELL_STATE);
        this->currState->set(i, DEF_CELL_STATE);
    }

    this->frameNum = 0;
}

Terminal::~Terminal() {
    delete this->renderState;
    delete this->currState;
}

void Terminal::render() {
    uint8_t slowBlink = this->frameNum % SLOW_BLINK_FREQ == 0;
    uint8_t slowBlinkStyle = (this->frameNum / SLOW_BLINK_FREQ) % 2;

    uint8_t fastBlink = this->frameNum % FAST_BLINK_FREQ == 0;
    uint8_t fastBlinkStyle = (this->frameNum / FAST_BLINK_FREQ) % 2;

    const uint8_t cellWidth = this->config.widthScale * 8; 
    const uint8_t cellHeight = (this->config.heightScale * 8) + (2 * this->config.pad);

    gfx_SetFontData(this->config.fontData);
    gfx_SetTextScale(this->config.widthScale, this->config.heightScale);
    gfx_SetMonospaceFont(cellWidth);

    cell_state_t *rState = this->renderState->getArrMut();
    cell_state_t *cState = this->currState->getArrMut();

    size_t i = 0;

    // NOTE: we will be using no clip and a byte sized y coord.
    // If you're terminal doesn't fit on screen, UB, sucks for you.
    uint8_t y = this->config.viewY;

    // Can always speed this up later tbh.
    for (uint8_t r = 0; r < this->config.rows; r++, y += cellHeight) {
        core::U24 x = 0;

        for (uint8_t c = 0; c < this->config.cols; c++, i++, x += cellWidth) {
            cell_state_t cCell = cState[i];

            uint8_t diffStates = this->renderStateVisible == 0 ||
                rState[i] != cCell;

            uint8_t fastBlinkRerender = (fastBlink && cState[i].style.fastBlink);
            uint8_t slowBlinkRerender = (slowBlink && cState[i].style.slowBlink);

            uint8_t renderNeeded = diffStates || fastBlinkRerender || slowBlinkRerender;

            if (!renderNeeded) {
                continue;
            }

            // Going to store new state before doing any rendering, just 
            // to make my life easier tbh.
            
            if (diffStates) {
                rState[i] = cCell;
            }

            // RENDER TIME!!

            // If there is padding, extra background will need to be rendered.
            if (this->config.pad > 0) {
                gfx_SetColor(this->config.colorMap[cCell.style.bgColor]);
                gfx_FillRectangle_NoClip(x, y, cellWidth, this->config.pad);
                gfx_FillRectangle_NoClip(x, y + cellHeight - this->config.pad, cellWidth, this->config.pad);
            }

            char buf[2] = {
                cCell.character, '\0'
            };

            if ((fastBlinkRerender && fastBlinkStyle) || 
                    (slowBlinkRerender && slowBlinkStyle)) {
                buf[0] = ' ';
            }

            // Now let's do fgRender.
            gfx_SetTextFGColor(this->config.colorMap[cCell.style.fgColor]);
            gfx_SetTextBGColor(this->config.colorMap[cCell.style.bgColor]);
            gfx_PrintStringXY(buf, x, y + this->config.pad);
        }
    }

    this->renderStateVisible = 1;
    this->frameNum++;
}

