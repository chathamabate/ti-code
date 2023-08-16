#include "./tree_pane.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil::gui;

TreePaneNode::TreePaneNode(uint8_t memChnl) 
    : core::SafeObject(memChnl) {
}

TreePaneNode::TreePaneNode() 
    : TreePaneNode(core::CXX_DEF_CHNL) {    
    this->parent = nullptr;
    this->ind = 0;
}

TreePaneNode::~TreePaneNode() {
}

void TreePaneNode::link(TreePaneNode *p, size_t i) {
    this->parent = p;
    this->ind = i;
}

TreePaneBranch::TreePaneBranch(core::SafeArray<TreePaneNode *> *chldn) 
    : TreePaneNode(chldn->getChnl()), children(chldn) {
    size_t len = this->children->getLen();

    // NOTE: This is the ONLY place where we link parents!
    for (size_t i = 0; i < len; i++) {
        this->children->get(i)->link(this, i);
    }

    this->expanded = false;
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

// Tree Pane GUI Stuff.

/*
TreePane::TreePane(uint8_t memChnl, const tree_pane_info_t *tpi, TreePaneNode *r) 
    : Pane(memChnl), paneInfo(tpi), root(r) {
}

TreePane::TreePane(const tree_pane_info_t *tpi, TreePaneNode *r) 
    : TreePane(core::CXX_DEF_CHNL, tpi, r) {
}

TreePane::~TreePane() {
}

void TreePane::render(uint24_t x, uint8_t y) const {
    // Render time my friend!!!
    // Honestly, I have no idea how this should be done.
    //
    // There could be like slots???
    // Which nodes are placed into from a stack???
    // Would be nice if we had a parent pointer ngl...
    // But then more work would need to go into this all???
    // But what about focusing... What will happen then???
    // RN with no text clipping, we don't need the fancy work of the
    // text scroll pane, most of the work kinda went no where tbh...
    //
    // Maybe we hold the index in each???
    // This might help us???
    // This will actually be enough...
    
    (void)x;
    (void)y;
}

void TreePane::update(core::KeyManager *km) {
    (void)km;
}*/
