
#include "cxxutil/core/mem.h"
#include "cxxutil/unitapp/unit_app.h"
#include "graphx.h"
#include "sys/lcd.h"
#include "ti/getcsc.h"
#include "ti/getkey.h"
#include <ti/screen.h>
#include <cxxutil/core/input.h>

#include <cxxutil/unit/unit.h>
#include <cxxutil/data/test/bits.h>
#include <cxxutil/data/test/heap.h>
#include <cxxutil/data/heap.h>

#include <cxxutil/data/bits.h>
#include <stdio.h>

#include "./model/maze.h"

using namespace cxxutil;


// Looking good so far!
static void viewBitGrid(cxxutil::data::BitGrid *bg) {
    const uint8_t SIDE_LEN = 8; // 5px per side.
    
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
    cxxutil::data::BitGrid *bg = 
        tif::model::createMaze(1, 15, 15);

    gfx_Begin();
    core::MemoryTracker::ONLY->setMER(core::GraphicsMemoryExitRoutine::ONLY);

    gfx_BlitScreen();
    gfx_SetDrawBuffer();

    // Make the whole screen white.
    gfx_FillScreen(255);
    viewBitGrid(bg);

    // Copy buffer to the screen.
    gfx_BlitBuffer();


    while (os_GetCSC() != sk_Clear);

    gfx_End();

    delete bg;

    core::MemoryTracker::ONLY->setMER(core::BasicMemoryExitRoutine::ONLY);
}

int main(void) {
    innerMain();

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}

