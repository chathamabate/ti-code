#include "./tree_pane.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/gui/tree_pane.h"
#include "cxxutil/unit/unit.h"

using namespace cxxutil;

// NOTE: This demonstrates the design I was going for 
// with TreePaneNodes.
//
// The user should never create a TreePaneLeaf or Branch
// explicitly, instead there should be a single function
// which handles the whole tree creation.
//
// The constructor of TreePaneBranch specifically is somewhat 
// dangerous.
//
// This way, there is no chance a node belongs to multiple trees.

static gui::TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr);

static const gui::tree_label_info_t TEST_TREE_LABEL_INFO = {
    .fgColor = 0,
    .bgColor = 255,
};

class TestBranch final : public gui::TreePaneBranch {
    friend TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr);

private:
    const core::SafeArray<char> * const label;

    TestBranch(const char *l, core::SafeArray<TreePaneNode *> *chldn) 
        : TreePaneBranch(chldn)
          , label(core::strCopySafe(chldn->getChnl(), l)) {
    }

public:
    virtual inline const gui::tree_label_info_t *getTreeLabelInfo() const override {
        // Not really necessary for tests.
        return &TEST_TREE_LABEL_INFO;
    }

    virtual inline const char *getLabel() const override {
        return this->label->getArr();
    }
};

class TestLeaf final : public gui::TreePaneLeaf {
    friend TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr);

private:
    const core::SafeArray<char> * const label;

    TestLeaf(uint8_t memChnl, const char *l) 
        : gui::TreePaneLeaf(memChnl), label(core::strCopySafe(memChnl, l)) {
    }

    TestLeaf(const char *l) 
        : TestLeaf(cxxutil::core::CXX_DEF_CHNL, l) {
    }

public:
    virtual inline const gui::tree_label_info_t *getTreeLabelInfo() const override {
        // Not really necessary for tests.
        return &TEST_TREE_LABEL_INFO;
    }

    virtual inline const char *getLabel() const override {
        return this->label->getArr();
    }
};

// UB if tStr is null or malformed. Postfix string to tree algo.
static gui::TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr) { 
    core::CoreList<gui::TreePaneNode *> *stack = 
        new core::CoreList<gui::TreePaneNode *>(memChnl);

    const char *iter = tStr;
    char lah;
    
    while ((lah = *(iter++)) != '\0') {
        const char lbl[2] = {lah, '\0'};

        if ('a' <= lah && lah <= 'z') { // Leaf Node.
            stack->add(new TestLeaf(memChnl, lbl));

            continue;
        }     
        
        size_t len = *(iter++) - '0';

        core::SafeArray<gui::TreePaneNode *> *children = 
            new core::SafeArray<gui::TreePaneNode *>(memChnl, len);

        for (size_t i = 0; i < len; i++) {
            children->set(i, stack->pop());
        }

        gui::TreePaneNode *node = new TestBranch(lbl, children); 

        stack->add(node);
    } 

    gui::TreePaneNode *result = stack->pop();

    delete stack;

    return result;
}

// NOTE: this test case basically does the reverse of 
// how each tree is built, then checks for equivelance.
class TreeTestCase : public unit::TestCase {
private:
    const char * const treeStr;

    gui::TreePaneNode *root;
    core::CoreList<gui::TreePaneNode *> *stack;

    void expandInStack(gui::TreePaneNode *r) {
        stack->add(r);
        
        if (r->isLeaf()) {
            return;
        }

        size_t len = r->getChildrenLen();
        gui::TreePaneNode * const *children = r->getChildren();

        for (size_t i = 0; i < len; i++) {
            this->expandInStack(children[i]);
        }
    }

    virtual void attempt(unit::TestContext *tc) override {
        this->root = createTestTree(1, this->treeStr);
        this->stack = new core::CoreList<gui::TreePaneNode *>(1);

        this->expandInStack(this->root); 

        // Now do the checks...
        const char *iter = this->treeStr;
        char lah;

        while ((lah = *(iter++)) != '\0') {
            tc->lblAssertTrue("Nodes Left", this->stack->getLen() > 0);

            gui::TreePaneNode *node = this->stack->pop();

            if ('a' <= lah && lah <= 'z') { 
                tc->lblAssertTrue("Is LNode", node->isLeaf());
                tc->lblAssertEqChar("LNode Lbl", lah, node->getLabel()[0]);
                tc->lblAssertEqUInt("LNode Len", 0, node->getChildrenLen());

                continue;
            }     

            // Branch node test.
            size_t len = *(iter++) - '0';

            tc->lblAssertTrue("Is BNode", node->isBranch());
            tc->lblAssertEqChar("BNode Lbl", lah, node->getLabel()[0]);
            tc->lblAssertEqUInt("BNode Len", len, node->getChildrenLen());
        }

        tc->lblAssertFalse("No Nodes Left", this->stack->getLen() > 0);
    }

    virtual void finally() override {
        delete stack;
        delete root;
    }
public:
    TreeTestCase(const char *n, const char *tStr) 
        : unit::TestCase(n), treeStr(tStr)  {
    }
};

static TreeTestCase SIMPLE1("Simple 1", "a");

const size_t TREE_PANE_NODE_SUITE_LEN = 1;
static unit::TestTree * const 
TREE_PANE_NODE_SUITE_TESTS[TREE_PANE_NODE_SUITE_LEN] = {
    &SIMPLE1,
};

static unit::TestSuite TREE_PANE_NODE_SUITE_VAL(
        "Tree Pane Node Suite", TREE_PANE_NODE_SUITE_TESTS, TREE_PANE_NODE_SUITE_LEN);

unit::TestTree * const gui::TREE_PANE_NODE_SUITE = &TREE_PANE_NODE_SUITE_VAL;

