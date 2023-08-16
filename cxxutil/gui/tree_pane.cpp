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

TreePane::TreePane(uint8_t memChnl, const tree_pane_info_t *tpi, TreePaneNode *r) 
    : Pane(memChnl), paneInfo(tpi), root(r) {
}

TreePane::TreePane(const tree_pane_info_t *tpi, TreePaneNode *r) 
    : TreePane(core::CXX_DEF_CHNL, tpi, r) {
}

TreePane::~TreePane() {
    delete this->root;
}

void TreePane::render(uint24_t x, uint8_t y) const {
    // Render time my friend!!!
}

void TreePane::update(core::KeyManager *km) {
}

uint24_t TreePane::getWidth() const {
    return this->paneInfo->width;
}

uint8_t TreePane::getHeight() const {
    return this->paneInfo->height;
}
