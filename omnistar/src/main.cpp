
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

#include "cxxutil/gui/test/centered_pane.h"
#include "cxxutil/unitapp/unit_app.h"
#include "keypadc.h"
#include "sys/timers.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <new>
#include <graphx.h>
#include <string.h>

#include <cutil/keys.h>
#include <cxxutil/core/mem.h>
#include <cxxutil/core/input.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>

#include <cxxutil/gui/test/text_block.h>
#include <cxxutil/gui/test/tree_pane.h>
#include <cxxutil/gui/test/scroll_text_pane.h>

using namespace cxxutil;

int main(void) {    
    unitapp::runUnitApp(gui::TEXT_BLOCK_SUITE);

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
