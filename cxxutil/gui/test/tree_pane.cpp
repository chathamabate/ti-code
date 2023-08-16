#include "./tree_pane.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/gui/tree_pane.h"

using namespace cxxutil::gui;

static TreePaneNode *createTestTree(const char *tStr);

static const tree_label_info_t TEST_TREE_LABEL_INFO = {
    .fgColor = 0,
    .bgColor = 255,
};

class TestBranch final : public TreePaneBranch {
    friend TreePaneNode *createTestTree(const char *tStr);

private:
    const char * const label;

    TestBranch(const char *l, cxxutil::core::SafeArray<TreePaneNode *> *chldn) 
        : TreePaneBranch(chldn), label(l) {
    }

public:
    virtual inline const tree_label_info_t *getTreeLabelInfo() const override {
        // Not really necessary for tests.
        return &TEST_TREE_LABEL_INFO;
    }

    virtual inline const char *getLabel() const override {
        return this->label;
    }
};

class TestLeaf final : public TreePaneLeaf {
    friend TreePaneNode *createTestTree(const char *tStr);

private:
    const char * const label;

    TestLeaf(uint8_t memChnl, const char *l) 
        : TreePaneLeaf(memChnl), label(l) {
    }

    TestLeaf(const char *l) 
        : TestLeaf(cxxutil::core::CXX_DEF_CHNL, l) {
    }

public:
    virtual inline const tree_label_info_t *getTreeLabelInfo() const override {
        // Not really necessary for tests.
        return &TEST_TREE_LABEL_INFO;
    }

    virtual inline const char *getLabel() const override {
        return this->label;
    }
};

static TreePaneNode *createTestTree(const char *tStr) {
    TestLeaf *l = new TestLeaf(1, "Hello");
    return l;
}
