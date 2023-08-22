#pragma once

#include <cxxutil/gui/pane.h>
#include <cxxutil/gui/tree_pane.h>
#include <cxxutil/unit/unit.h>

namespace cxxutil { namespace gui {
    extern unit::TestTree * const TREE_PANE_NODE_SUITE;
    extern unit::TestTree * const TREE_PANE_UI_SUITE;

    // NOTE: Tree Pane Suite contains the above two suites.
    extern unit::TestTree * const TREE_PANE_SUITE;

    void TreePaneGUITest();
}}
