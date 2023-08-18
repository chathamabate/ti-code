#include "./tree_pane.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil::gui;

TreePaneNode::TreePaneNode(uint8_t memChnl) 
    : core::SafeObject(memChnl) {
    this->parent = nullptr;
    this->ind = 0;
    this->depth = 0;
}

TreePaneNode::TreePaneNode() 
    : TreePaneNode(core::CXX_DEF_CHNL) {    
}

TreePaneNode::~TreePaneNode() {
}

void TreePaneNode::link(TreePaneNode *p, size_t i) {
    this->parent = p;
    this->ind = i;
}

TreePaneNode *TreePaneNode::nextUp() {
    if (this->ind == 0) {
        return this->parent;
    }

    // It is assumed that parent must be expanded.
    TreePaneNode *iter = 
        this->parent->getChildren()[this->ind - 1];

    while (!(iter->isLeaf() || iter->isMinimized())) {
        iter = iter->getChildren()[iter->getChildrenLen() - 1];
    }

    return iter;
}

TreePaneNode *TreePaneNode::nextDown() {
    if (this->isBranch() && this->isExpanded()) {
        return this->getChildren()[0];
    }

    TreePaneNode *iter = this;

    while (iter->parent) {
        // If the iterator is not the leftmost child.
        if (iter->ind < iter->parent->getChildrenLen() - 1) {
            return iter->parent->getChildren()[iter->ind + 1];
        }
    }

    // We made it all the way to the root!
    return nullptr;
}

void TreePaneBranch::setDepth(size_t d) {
    this->depth = d;

    size_t len = this->children->getLen();
    for (size_t i = 0; i < len; i++) {
        this->children->get(i)->setDepth(d + 1);
    }
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

void TreePaneLeaf::setDepth(size_t d) {
    this->depth = d;
}

TreePaneLeaf::TreePaneLeaf(uint8_t memChnl) 
    : TreePaneNode(memChnl) {
}
 
TreePaneLeaf::TreePaneLeaf() 
    : TreePaneLeaf(core::CXX_DEF_CHNL) {
}

TreePaneLeaf::~TreePaneLeaf() {
}

// Tree Pane GUI Stuff.

TreePane::TreePane(uint8_t memChnl, const tree_pane_info_t *tpi, TreePaneNode *r) 
    : Pane(memChnl), paneInfo(tpi), root(r) {
    this->sel = r;
    this->selRelY = 0;
}

TreePane::TreePane(const tree_pane_info_t *tpi, TreePaneNode *r) 
    : TreePane(core::CXX_DEF_CHNL, tpi, r) {
}

TreePane::~TreePane() {
}

void TreePane::render(uint24_t x, uint8_t y) const {
    (void)x;
    (void)y;
}

void TreePane::update(core::KeyManager *km) {
    (void)km;
}

void TreePane::scrollDown() {

}

void TreePane::scrollUp() {

}
