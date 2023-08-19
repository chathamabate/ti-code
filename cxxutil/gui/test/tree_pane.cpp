#include "./tree_pane.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/gui/tree_pane.h"
#include "cxxutil/gui/text_block.h"
#include "cxxutil/unit/unit.h"
#include "ti/screen.h"

#include <string.h>

using namespace cxxutil;

// NOTE: This demonstrates the design I was going for 
// with TreePaneNodes.
//
// The user should never create a TreePaneLeaf or Branch
// explicitly, instead there should be a single function
// which handles the whole tree creation.
//
// This way, there is no chance a node belongs to multiple trees.

static gui::TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr);

static const gui::text_color_info_t TEST_TREE_LABEL_INFO = {
    .fgColor = 0,
    .bgColor = 255,
};

class TestBranch final : public gui::TreePaneBranch {
    friend TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr);

private:
    char lbl[2];

    TestBranch(char l, core::SafeArray<TreePaneNode *> *chldn) 
        : TreePaneBranch(chldn) {
        this->lbl[0] = l;
        this->lbl[1] = '\0';
    }

public:
    virtual inline const gui::text_color_info_t *getTreeLabelInfo() const override {
        return &TEST_TREE_LABEL_INFO;
    }

    virtual inline const char *getLabel() const override {
        return this->lbl;
    }
};

class TestLeaf final : public gui::TreePaneLeaf {
    friend TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr);

private:
    char lbl[2];

    TestLeaf(uint8_t memChnl, char l) 
        : gui::TreePaneLeaf(memChnl) {
        this->lbl[0] = l;
        this->lbl[1] = '\0';
    }

    TestLeaf(char l) 
        : TestLeaf(cxxutil::core::CXX_DEF_CHNL, l) {
    }

public:
    virtual inline const gui::text_color_info_t *getTreeLabelInfo() const override {
        return &TEST_TREE_LABEL_INFO;
    }

    virtual inline const char *getLabel() const override {
        return this->lbl;
    }
};

// UB if tStr is null or malformed. Postfix string to tree algo.
// 
// String must be non-null and non-empty.
// See TestCases for example tree building strings.
static gui::TreePaneNode *createTestTree(uint8_t memChnl, const char *tStr) { 
    core::CoreList<gui::TreePaneNode *> *stack = 
        new core::CoreList<gui::TreePaneNode *>(memChnl);

    const char *iter = tStr;
    
    while (*iter != '\0') {
        char lbl = *(iter++);
        if ('a' <= lbl && lbl <= 'z') { // Leaf Node.
            stack->add(new TestLeaf(memChnl, lbl));

            continue;
        }     

        char exp, num;
        bool expanded = false;

        exp = *(iter++); 
        if (exp == '*') {
            expanded = true;
            num = *(iter++);
        } else {
            num = exp;
        }

        size_t len = num - '0';

        core::SafeArray<gui::TreePaneNode *> *children = 
            new core::SafeArray<gui::TreePaneNode *>(memChnl, len);

        for (size_t i = 0; i < len; i++) {
            children->set(i, stack->pop());
        }

        gui::TreePaneNode *node = new TestBranch(lbl, children); 
        node->setExpanded(expanded);

        stack->add(node);
    } 

    gui::TreePaneNode *result = stack->pop();
    result->declareRoot();

    delete stack;

    return result;
}

// NOTE: this test case basically does the reverse of 
// how each tree is built, then checks for equivelance.
class TreeStructureTestCase : public unit::TestCase {
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

    // NOTE: Consider adding better assertion labeling here.

    void assertExpParents(unit::TestContext *tc, gui::TreePaneNode *r, size_t d) {
        tc->lblAssertEqUInt("Depth", d, r->getDepth());

        if (r->isLeaf()) {
            return;
        }

        size_t len = r->getChildrenLen();
        gui::TreePaneNode * const *children = r->getChildren();

        for (size_t i = 0; i < len; i++) {
            gui::TreePaneNode *child = children[i];

            this->assertExpParents(tc, child, d + 1);

            tc->lblAssertEqChar("Parent Label", 
                    r->getLabel()[0],
                    child->getParent()->getLabel()[0]);

            tc->lblAssertEqUInt("Parent Ind",
                    i, 
                    child->getIndex());
        }
    }

    virtual void attempt(unit::TestContext *tc) override {
        this->root = createTestTree(1, this->treeStr);

        // First we use a stack to confirm the structure of
        // the tree matches that of the given string.
        this->stack = new core::CoreList<gui::TreePaneNode *>(1);

        this->expandInStack(this->root); 

        const char *iter = this->treeStr;

        while (*iter != '\0') {
            tc->lblAssertTrue("Nodes Left", this->stack->getLen() > 0);
            gui::TreePaneNode *node = this->stack->pop();

            char lbl = *(iter++);

            if ('a' <= lbl && lbl <= 'z') { 
                tc->lblAssertTrue("Is LNode", node->isLeaf());
                tc->lblAssertEqChar("LNode Lbl", lbl, node->getLabel()[0]);
                tc->lblAssertEqUInt("LNode Len", 0, node->getChildrenLen());

                continue;
            }     

            char exp, num;
            bool expanded = false;

            exp = *(iter++);

            if (exp == '*') {
                expanded = true;
                num = *(iter++);
            } else {
                num = exp;
            }

            // Branch node test.
            size_t len = num - '0';

            tc->lblAssertTrue("Is BNode", node->isBranch());
            tc->lblAssertEqChar("BNode Lbl", lbl, node->getLabel()[0]);
            tc->lblAssertEqUInt("BNode Len", len, node->getChildrenLen());
            tc->lblAssertTrue("BNode Exp", expanded == node->isExpanded());
        }

        tc->lblAssertFalse("No Nodes Left", this->stack->getLen() > 0);

        // Finally, let's make sure the parent pointers, indeces, and depths
        // are all correct.
        
        this->assertExpParents(tc, this->root, 0);
    }

    virtual void finally() override {
        delete stack;
        delete root;
    }
public:
    TreeStructureTestCase(const char *n, const char *tStr) 
        : unit::TestCase(n), treeStr(tStr)  {
    }
};

// Structure Tests.
static TreeStructureTestCase STRUCT_SIMPLE1("Structure Simple 1", "a");
static TreeStructureTestCase STRUCT_SIMPLE2("Structure Simple 2", "aA*1");
static TreeStructureTestCase STRUCT_SIMPLE3("Structure Simple 3", "abcA*3");
static TreeStructureTestCase STRUCT_SIMPLE4("Structure Simple 4", "aaA2bB*1ccC2A*3");

static TreeStructureTestCase STRUCT_BIG1("Structure Big 1", "aA1bbB*2cD3jjjJ3tT1pP2S*3");
static TreeStructureTestCase STRUCT_BIG2("Structure Big 2", "uuU*2hH1L2patT*1N*2syyY2S3bB1M1Q4");

class FlexibleTreeTestCase : public unit::TestCase {
private:
    virtual void attempt(unit::TestContext *tc) override {
        this->root = createTestTree(1, this->treeStr);
        this->attemptBody(tc);
    }

    virtual void finally() override {
        delete this->root;
    }

    virtual void attemptBody(unit::TestContext *tc) = 0;

protected:
    const char * const treeStr;
    gui::TreePaneNode *root;

public:
    FlexibleTreeTestCase(const char *n, const char *tStr) 
        : unit::TestCase(n), treeStr(tStr) {
    }
};

class NextUpDownTestCase : public FlexibleTreeTestCase {
private:
    const char * const expPath;

    virtual void attemptBody(unit::TestContext *tc) override {
        gui::TreePaneNode *currNode = nullptr;
        gui::TreePaneNode *nextNode = this->root;   // should never be null.

        // NOTE: the given string will actually be read in reverse
        // to make specifing tests easier. SEE test cases.
        const char *charIter = this->expPath + strlen(this->expPath); 

        // Scroll Down path first.
        while (nextNode) {
            currNode = nextNode;

            if (--charIter < this->expPath) {
                tc->fatal("NextD Exceed");
            }

            char expLbl = *charIter;

            // Actual path doesn't match expected path.
            tc->lblAssertEqChar("NextD Miss", expLbl, currNode->getLabel()[0]);

            nextNode = currNode->nextDown();
        }

        // The path ends before the expected path ends.
        if (charIter != this->expPath) {
            tc->fatal("NextD Early");
        }

        // At this point curr the last "nextDown"'d node in the
        // tree. Now, we must nextUp all the way back to the root.

        nextNode = currNode;
        currNode = nullptr;
        
        while (nextNode) {
            currNode = nextNode;

            char expLbl = *(charIter++);

            if (expLbl == '\0') {
                tc->fatal("NextU Exceed");
            }

            tc->lblAssertEqChar("NextU Miss", expLbl, currNode->getLabel()[0]);

            nextNode = currNode->nextUp();
        }

        if (*charIter != '\0') {
            tc->fatal("NextU Early");
        }
    }

public:
    // The given test string must construct a non-empty tree.
    NextUpDownTestCase(const char *n, const char *tStr, const char *exp) 
        : FlexibleTreeTestCase(n, tStr), expPath(exp) {
    }
};

static NextUpDownTestCase UD_SIMPLE1("UD Simple 1", "a", "a");
static NextUpDownTestCase UD_SIMPLE2("UD Simple 2", "aA*1", "aA");
static NextUpDownTestCase UD_SIMPLE3("UD Simple 3", "abA*2", "abA");
static NextUpDownTestCase UD_SIMPLE4("UD Simple 4", "abA2", "A");
static NextUpDownTestCase UD_SIMPLE5("UD Simple 5", "abA*2cdD*2eE1F*3", "abAcdDEF");

static NextUpDownTestCase UD_BIG1("UD Big 1", 
        "abA*2lstK3M*1P*2nD*1plQ1S*2Z*3", "abAKMPnDpQSZ");

const size_t TREE_PANE_NODE_SUITE_LEN = 12;
static unit::TestTree * const 
TREE_PANE_NODE_SUITE_TESTS[TREE_PANE_NODE_SUITE_LEN] = {
    &STRUCT_SIMPLE1,
    &STRUCT_SIMPLE2,
    &STRUCT_SIMPLE3,
    &STRUCT_SIMPLE4,

    &STRUCT_BIG1,
    &STRUCT_BIG2,

    &UD_SIMPLE1,
    &UD_SIMPLE2,
    &UD_SIMPLE3,
    &UD_SIMPLE4,
    &UD_SIMPLE5,

    &UD_BIG1,
};

static unit::TestSuite TREE_PANE_NODE_SUITE_VAL(
        "Tree Pane Node Suite", TREE_PANE_NODE_SUITE_TESTS, TREE_PANE_NODE_SUITE_LEN);

unit::TestTree * const gui::TREE_PANE_NODE_SUITE = &TREE_PANE_NODE_SUITE_VAL;

