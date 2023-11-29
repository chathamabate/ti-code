
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
    // TODO: write grid drawing algo.
}

void innerMain(void) {
    cxxutil::data::BitGrid *bg = 
        tif::model::createMaze(1, 10, 10);

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

