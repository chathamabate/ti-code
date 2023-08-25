#pragma once

#include <cxxutil/unit/unit.h>
#include <cxxutil/core/mem.h>
#include <cxxutil/gui/tree_pane.h>
#include <cxxutil/gui/scroll_text_pane.h>


namespace cxxutil { namespace unitapp {
    // NOTE: this function temporarily overwrites the memory exit routine. 
    // The MER present at the start of the function call will be restored
    // on return.
    void runUnitApp(unit::TestTree *testTree);
}}
