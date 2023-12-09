
#include "cxxutil/core/mem.h"
#include "cxxutil/unitapp/unit_app.h"
#include "graphx.h"
#include "sys/lcd.h"
#include "ti/getcsc.h"
#include "ti/getkey.h"
#include <cstdlib>
#include <ti/screen.h>
#include <cxxutil/core/input.h>

#include <cxxutil/unit/unit.h>
#include <cxxutil/data/test/bits.h>
#include <cxxutil/data/test/heap.h>
#include <cxxutil/data/heap.h>

#include <cxxutil/data/bits.h>
#include <stdio.h>
#include <time.h>

#include "./model/maze.h"
#include "./model/test/maze.h"
#include "./model/test/state.h"

using namespace cxxutil;

// Looking good so far!
static void viewBitGrid(cxxutil::data::BitGrid *bg) {
    const uint8_t SIDE_LEN = 4; // 5px per side.
    
    const uint8_t PATH_COLOR = 22; 
    const uint8_t BARR_COLOR = 19;
                                
    size_t rowBound = bg->getRows();
    if (rowBound * SIDE_LEN > GFX_LCD_HEIGHT) {
        rowBound = GFX_LCD_HEIGHT / SIDE_LEN;
    }
                                
    size_t colBound = bg->getCols();
    if (colBound * SIDE_LEN > GFX_LCD_WIDTH) {
        colBound = GFX_LCD_WIDTH / SIDE_LEN;
    }

    gfx_SetColor(PATH_COLOR);
    gfx_FillRectangle(0, 0, 
            colBound * SIDE_LEN, rowBound * SIDE_LEN);

    gfx_SetColor(BARR_COLOR);

    for (size_t r = 0; r < rowBound; r++) {
        for (size_t c = 0; c < colBound; c++) {
            if (!(bg->get(r, c))) {
                gfx_FillRectangle(c * SIDE_LEN, r * SIDE_LEN, 
                        SIDE_LEN, SIDE_LEN);
            }
        }
    } 
}

void innerMain(void) {
    time_t t;
    srand(time(&t));

    cxxutil::data::BitGrid *bg = new cxxutil::data::BitGrid(2, 5, 5);

    cxxutil::data::BitGrid *v = tif::model::createMazeDFS(2, bg);

    gfx_Begin();
    core::MemoryTracker::ONLY->setMER(core::GraphicsMemoryExitRoutine::ONLY);

    gfx_BlitScreen();
    gfx_SetDrawBuffer();

    // Make the whole screen white.
    gfx_FillScreen(255);
    viewBitGrid(v);

    // Copy buffer to the screen.
    gfx_BlitBuffer();


    while (os_GetCSC() != sk_Clear);

    gfx_End();

    delete v;
    delete bg;

    core::MemoryTracker::ONLY->setMER(core::BasicMemoryExitRoutine::ONLY);
}

int main(void) {
    // innerMain();
    unitapp::runUnitApp(tif::model::STATE_SUITE);

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}

