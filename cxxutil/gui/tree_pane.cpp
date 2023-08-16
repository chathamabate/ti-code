#include "./tree_pane.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil::gui;

TreePaneNode::TreePaneNode(uint8_t memChnl) 
    : core::SafeObject(memChnl) {
}

TreePaneNode::TreePaneNode() 
    : TreePaneNode(core::CXX_DEF_CHNL) {    
}

TreePaneNode::~TreePaneNode() {
}

TreePaneBranch::TreePaneBranch(core::SafeArray<TreePaneNode *> *chldn) 
    : TreePaneNode(chldn->getChnl()), children(chldn) {
}

TreePaneBranch::~TreePaneBranch() {
    size_t len = this->children->getLen(); 
    for (size_t i = 0; i < len; i++) {
        delete this->children->get(i);
    }

    delete this->children;
}

TreePaneLeaf::TreePaneLeaf(uint8_t memChnl) 
    : TreePaneNode(memChnl) {
}
 
TreePaneLeaf::TreePaneLeaf() 
    : TreePaneLeaf(core::CXX_DEF_CHNL) {

}
