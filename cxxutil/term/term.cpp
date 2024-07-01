
#include "./term.h"
#include "cxxutil/core/mem.h"
#include <graphx.h>

using namespace cxxutil::term;

const cell_state_t cxxutil::term::DEF_CELL_STATE = {
    .style = {
        .fgColor = ANSII_STRONG_WHITE,
        .bgColor = ANSII_STRONG_BLACK,
        .underline = 0,
        .slowBlink = 1,
        .fastBlink = 0,
        .strikeThru = 0,
    },
    .character = 'A'
};

const uint8_t cxxutil::term::DEF_COLOR_MAP[17] = {
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

    // Transparent color.
    222,
};

Terminal::Terminal(uint8_t bufrd, const terminal_config_t &cfg) 
    : Terminal(core::CXX_DEF_CHNL, bufrd, cfg) {
}

Terminal::Terminal(uint8_t chnl, uint8_t bufrd, const terminal_config_t &cfg) 
    : core::SafeObject(chnl), buffered(bufrd) {
    const size_t numCells = this->config.rows * this->config.cols;

    this->config = cfg;
    this->currState = new core::SafeArray<cell_state_t>(chnl, numCells);

    this->renders[0].visible = 0;
    this->renders[0].state = new core::SafeArray<cell_state_t>(chnl, numCells);

    if (this->buffered) {
        this->renders[1].visible = 0;
        this->renders[1].state = new core::SafeArray<cell_state_t>(chnl, numCells);
    }

    for (size_t i = 0; i < numCells; i++) {
        this->currState->set(i, DEF_CELL_STATE);

        this->renders[0].state->set(i, DEF_CELL_STATE);

        if (this->buffered) {
            this->renders[1].state->set(i, DEF_CELL_STATE);
        }
    }

    this->frameNum = 0;
}

Terminal::~Terminal() {
    delete this->currState;

    delete this->renders[0].state;
    if (this->buffered) {
        delete this->renders[1].state;
    }
}


void Terminal::render() {
    // NOTE: that the current state can exist without an 
    // associated render.
    cell_state_t *cState = this->currState->getArrMut();

    uint8_t ri = this->buffered 
        ? this->frameNum % 2 : 0;

    // The frame number of the render state we may be painting over.
    uint8_t lastFrameNum = this->buffered 
        ? this->frameNum - 2 : this->frameNum - 1;

    uint8_t fullRenderNeeded = this->renders[ri].visible == 0;
    cell_state_t *rState = this->renders[ri].state->getArrMut();

    // NOTE: slowBlinkRedrawNeeded means that a cell marked slow blink
    // will need to be redrawn if there is no difference between the 
    // state of the render we are painting over and the actual state.
    uint8_t cSlowBlinkStyle = (this->frameNum / SLOW_BLINK_FREQ) % 2;
    uint8_t rSlowBlinkStyle = (lastFrameNum / SLOW_BLINK_FREQ) % 2;
    uint8_t slowBlinkRedrawNeeded = (cSlowBlinkStyle != rSlowBlinkStyle);

    uint8_t cFastBlinkStyle = (this->frameNum / FAST_BLINK_FREQ) % 2;
    uint8_t rFastBlinkStyle = (lastFrameNum / FAST_BLINK_FREQ) % 2;
    uint8_t fastBlinkRedrawNeeded = (cFastBlinkStyle != rFastBlinkStyle);

    const uint8_t cellWidth = this->config.widthScale * 8; 
    const uint8_t cellHeight = (this->config.heightScale * 8) + (2 * this->config.pad);

    gfx_SetFontData(this->config.fontData);
    gfx_SetTextScale(this->config.widthScale, this->config.heightScale);
    gfx_SetMonospaceFont(cellWidth);
    gfx_SetTextTransparentColor(this->config.colorMap[TRANSPARENT_IND]);

    size_t i = 0;

    // NOTE: we will be using no clip and a byte sized y coord.
    // If you're terminal doesn't fit on screen, UB, sucks for you.
    uint8_t y = this->config.viewY;

    // Can always speed this up later tbh.
    for (uint8_t r = 0; r < this->config.rows; r++, y += cellHeight) {
        core::U24 x = 0;

        for (uint8_t c = 0; c < this->config.cols; c++, i++, x += cellWidth) {
            cell_state_t cCell = cState[i];

            uint8_t diffStates = rState[i] != cCell;

            // Any cell which has fast blink style will need to be rerendered,
            // EVEN if it has the seemlingly same state of the render we are painting over.
            uint8_t fastBlinkRerender = (fastBlinkRedrawNeeded && cCell.style.fastBlink);
            uint8_t slowBlinkRerender = (slowBlinkRedrawNeeded && cCell.style.slowBlink);

            uint8_t renderNeeded = fullRenderNeeded || diffStates || fastBlinkRerender || slowBlinkRerender;

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

            gfx_SetTextFGColor(this->config.colorMap[cCell.style.fgColor]);
            gfx_SetTextBGColor(this->config.colorMap[cCell.style.bgColor]);

            char buf[2] = {
                cCell.character, '\0'
            };

            if ((cCell.style.fastBlink && cFastBlinkStyle) || 
                    (cCell.style.slowBlink && cSlowBlinkStyle)) {
                buf[0] = ' ';
            }

            // Now let's do fgRender.
            gfx_PrintStringXY(buf, x, y + this->config.pad);
        }
    }

    this->renders[ri].visible = 1;

    this->frameNum++;
}

