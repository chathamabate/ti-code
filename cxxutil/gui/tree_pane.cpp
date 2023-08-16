#include "./tree_pane.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil;

gui::TreePaneNode::TreePaneNode(uint8_t memChnl) 
    : core::SafeObject(memChnl) {
}

gui::TreePaneNode::TreePaneNode() 
    : gui::TreePaneNode(core::CXX_DEF_CHNL) {
}

gui::TreePaneNode::~TreePaneNode() {
}
