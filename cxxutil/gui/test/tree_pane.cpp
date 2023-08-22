#include "./tree_pane.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/input.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/gui/tree_pane.h"
#include "cxxutil/gui/text_block.h"
#include "cxxutil/unit/unit.h"
#include "graphx.h"
#include "ti/screen.h"

#include <string.h>

using namespace cxxutil;

class LetterState : public core::SafeObject {
private:
    static const gui::text_color_info_t LS_INFO;

    char lbl[7];
public:
    LetterState(uint8_t memChnl, char l) 
        : core::SafeObject(memChnl), lbl("Lbl: ") {
        lbl[5] = l;
        lbl[6] = '\0';
    }

    // Custom methods.
    
    inline char getChar() const {
        return this->lbl[0];
    }

    // Required methods.
    
    inline const char *getLabel() const {
        return this->lbl;
    }

    inline const gui::text_color_info_t *getLabelInfo() const {
        return &(LetterState::LS_INFO);
    }
};

const gui::text_color_info_t LetterState::LS_INFO = {
    .fgColor = 0,
    .bgColor = 255,
};

// NOTE: This demonstrates the design I was going for 
// with TreePaneNodes.
//
// The user should never create a TreePaneLeaf or Branch
// explicitly, instead there should be a single function
// which handles the whole tree creation.
//
// This way, there is no chance a node belongs to multiple trees.

static gui::TreePaneNode<LetterState> *createTestTree(uint8_t memChnl, const char *tStr);

// UB if tStr is null or malformed. Postfix string to tree algo.
// 
// String must be non-null and non-empty.
// See TestCases for example tree building strings.
static gui::TreePaneNode<LetterState> *createTestTree(uint8_t memChnl, const char *tStr) { 
    core::CoreList<gui::TreePaneNode<LetterState> *> *stack = 
        new core::CoreList<gui::TreePaneNode<LetterState> *>(memChnl);

    const char *iter = tStr;
    
    while (*iter != '\0') {
        char lbl = *(iter++);
        LetterState *ls = new LetterState(memChnl, lbl);

        if ('a' <= lbl && lbl <= 'z') { // Leaf Node.
                                        
            stack->add(new gui::TreePaneLeaf<LetterState>(memChnl, ls));

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

        core::SafeArray<gui::TreePaneNode<LetterState> *> *children = 
            new core::SafeArray<gui::TreePaneNode<LetterState> *>(memChnl, len);

        for (size_t i = 0; i < len; i++) {
            children->set(i, stack->pop());
        }

        gui::TreePaneNode<LetterState> *node = 
            new gui::TreePaneBranch<LetterState>(ls, children); 

        node->setExpanded(expanded);

        stack->add(node);
    } 

    gui::TreePaneNode<LetterState> *result = stack->pop();
    result->declareRoot();

    delete stack;

    return result;
}

// NOTE: this test case basically does the reverse of 
// how each tree is built, then checks for equivelance.
class TreeStructureTestCase : public unit::TestCase {
private:
    const char * const treeStr;

    gui::TreePaneNode<LetterState> *root;
    core::CoreList<gui::TreePaneNode<LetterState> *> *stack;

    void expandInStack(gui::TreePaneNode<LetterState> *r) {
        stack->add(r);
        
        if (r->isLeaf()) {
            return;
        }

        size_t len = r->getChildrenLen();
        gui::TreePaneNode<LetterState> * const *children = r->getChildren();

        for (size_t i = 0; i < len; i++) {
            this->expandInStack(children[i]);
        }
    }

    // NOTE: Consider adding better assertion labeling here.

    void assertExpParents(unit::TestContext *tc, 
            gui::TreePaneNode<LetterState> *r, size_t d) {
        tc->lblAssertEqUInt("Depth", d, r->getDepth());

        if (r->isLeaf()) {
            return;
        }

        size_t len = r->getChildrenLen();
        gui::TreePaneNode<LetterState> * const *children = r->getChildren();

        for (size_t i = 0; i < len; i++) {
            gui::TreePaneNode<LetterState> *child = children[i];

            this->assertExpParents(tc, child, d + 1);

            tc->lblAssertEqChar("Parent Label", 
                    r->getState()->getChar(),
                    child->getParent()->getState()->getChar());

            tc->lblAssertEqUInt("Parent Ind",
                    i, 
                    child->getIndex());
        }
    }

    virtual void attempt(unit::TestContext *tc) override {
        this->root = createTestTree(1, this->treeStr);

        // First we use a stack to confirm the structure of
        // the tree matches that of the given string.
        this->stack = new core::CoreList<gui::TreePaneNode<LetterState> *>(1);

        this->expandInStack(this->root); 

        const char *iter = this->treeStr;

        while (*iter != '\0') {
            tc->lblAssertTrue("Nodes Left", this->stack->getLen() > 0);
            gui::TreePaneNode<LetterState> *node = this->stack->pop();

            char lbl = *(iter++);

            if ('a' <= lbl && lbl <= 'z') { 
                tc->lblAssertTrue("Is LNode", node->isLeaf());
                tc->lblAssertEqChar("LNode Lbl", lbl, node->getState()->getChar());
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
            tc->lblAssertEqChar("BNode Lbl", lbl, node->getState()->getChar());
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
    gui::TreePaneNode<LetterState> *root;

public:
    FlexibleTreeTestCase(const char *n, const char *tStr) 
        : unit::TestCase(n), treeStr(tStr) {
    }
};

class NextUpDownTestCase : public FlexibleTreeTestCase {
private:
    const char * const expPath;

    virtual void attemptBody(unit::TestContext *tc) override {
        gui::TreePaneNode<LetterState> *currNode = nullptr;
        gui::TreePaneNode<LetterState> *nextNode = this->root;   // should never be null.

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
            tc->lblAssertEqChar("NextD Miss", expLbl, 
                    currNode->getState()->getChar());

            nextNode = currNode->nextDown();
        }

        // The path ends before the expected path ends.
        if (charIter != this->expPath) {
            tc->fatal("NextD Early");
        }

        // At this point curr the last "nextDown"'d node in the
        // tree. Now, we must nextUp all the way back to the root.

        
        while (currNode) {
            char expLbl = *(charIter++);

            if (expLbl == '\0') {
                tc->fatal("NextU Exceed");
            }

            tc->lblAssertEqChar("NextU Miss", expLbl, 
                    currNode->getState()->getChar());

            currNode = currNode->nextUp();
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

class ReachableTreeTestCase : public FlexibleTreeTestCase {
private:
    const size_t expReachable;

    virtual void attemptBody(unit::TestContext *tc) override  {
        tc->assertEqUInt(this->expReachable, this->root->getNumReachable());
    }
public:
    ReachableTreeTestCase(const char *n, const char *tStr, size_t r) 
        : FlexibleTreeTestCase(n, tStr), expReachable(r) {
    }
};

static ReachableTreeTestCase R_SIMPLE1("R Simple 1", "a", 1);
static ReachableTreeTestCase R_SIMPLE2("R Simple 2", "aA1", 1);
static ReachableTreeTestCase R_SIMPLE3("R Simple 3", "aA*1", 2);
static ReachableTreeTestCase R_SIMPLE4("R Simple 4", "aaA*2", 3);
static ReachableTreeTestCase R_SIMPLE5("R Simple 5", "aaA2bB*1cD*3", 5);

const size_t TREE_PANE_NODE_SUITE_LEN = 17;
static unit::TestTree * const 
TREE_PANE_NODE_SUITE_TESTS[TREE_PANE_NODE_SUITE_LEN] = {
    // NOTE: consider making this separate test trees later.
    
    // Structure Tests.
    &STRUCT_SIMPLE1,
    &STRUCT_SIMPLE2,
    &STRUCT_SIMPLE3,
    &STRUCT_SIMPLE4,

    &STRUCT_BIG1,
    &STRUCT_BIG2,

    // nextUp/nextDown tests.
    &UD_SIMPLE1,
    &UD_SIMPLE2,
    &UD_SIMPLE3,
    &UD_SIMPLE4,
    &UD_SIMPLE5,

    &UD_BIG1,

    // Num Reachable tests.
    &R_SIMPLE1,
    &R_SIMPLE2,
    &R_SIMPLE3,
    &R_SIMPLE4,
    &R_SIMPLE5,
};

static unit::TestSuite TREE_PANE_NODE_SUITE_VAL(
        "Tree Pane Node Suite", TREE_PANE_NODE_SUITE_TESTS, TREE_PANE_NODE_SUITE_LEN);

unit::TestTree * const gui::TREE_PANE_NODE_SUITE = &TREE_PANE_NODE_SUITE_VAL;

// Now time for TreePane UI tests.

class FlexibleTreePaneTestCase : public unit::TestCase {
private:
    virtual void attempt(unit::TestContext *tc) override {
        this->root = createTestTree(1, this->treeStr);
        this->pane = new gui::TreePane<LetterState>(1, this->treePaneInfo, this->root);

        this->attemptBody(tc);
    }

    virtual void finally() override {
        delete this->pane;
        delete this->root;
    }

    virtual void attemptBody(unit::TestContext *tc) = 0;

protected:
    const char * const treeStr;
    const gui::tree_pane_info_t * const treePaneInfo;

    gui::TreePaneNode<LetterState> *root;
    gui::TreePane<LetterState> *pane;

public:
    FlexibleTreePaneTestCase(const char *n, const gui::tree_pane_info_t *tpi, const char *tStr) 
        : unit::TestCase(n), treeStr(tStr), treePaneInfo(tpi) {
    }
};

static const gui::tree_pane_info_t TPI1 = {
    .height = 30, // A little more than three rows.
    .lblVertSpace = 2,
    .lblHeightScale = 1,
};

class TreePaneTestCase1 : public FlexibleTreePaneTestCase {
private:
    static TreePaneTestCase1 ONLY_VAL;
    TreePaneTestCase1() : FlexibleTreePaneTestCase("TreePane 1", 
            &TPI1, "abcA3dD*1E*2") {}

    virtual void attemptBody(unit::TestContext *tc) override {
        tc->lblAssertEqUInt("4L.0", 4, this->pane->getTotalRows());
        
        this->pane->scrollDown(); 

        tc->lblAssertEqUInt("4L.1 RI", 1, this->pane->getSelectedRowInd());
        tc->lblAssertEqUInt("4L.1 RrY", 10, this->pane->getSelectedRowRelY());
        
        this->pane->scrollDown();
        this->pane->scrollDown();

        tc->lblAssertEqUInt("4L.2 RI", 3, this->pane->getSelectedRowInd());
        tc->lblAssertEqUInt("4L.2 RrY", 30 - 8, this->pane->getSelectedRowRelY());

        this->pane->toggle();

        tc->lblAssertEqUInt("7L.0", 7, this->pane->getTotalRows());

        this->pane->scrollUp();

        tc->lblAssertEqUInt("7L.1 RI", 2, this->pane->getSelectedRowInd());
        tc->lblAssertEqUInt("7L.1 RrY", 30 - 18, this->pane->getSelectedRowRelY());

        this->pane->scrollUp();
        this->pane->toggle();

        tc->lblAssertEqUInt("6L.0", 6, this->pane->getTotalRows());

        this->pane->scrollUp();
        tc->lblAssertEqUInt("6L.1 RrY", 0, this->pane->getSelectedRowRelY());
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

TreePaneTestCase1 TreePaneTestCase1::ONLY_VAL;

static gui::tree_pane_info_t TPI2 = {
    .height = 8, // Only space for one row.
                 
    .lblVertSpace = 2,
    .lblHeightScale = 1,
};

class TreePaneTestCase2 : public FlexibleTreePaneTestCase {
private:
    static TreePaneTestCase2 ONLY_VAL;
    TreePaneTestCase2() : FlexibleTreePaneTestCase("TreePane 2", 
            &TPI2, "abcA*3") {}

    virtual void attemptBody(unit::TestContext *tc) override {
        this->pane->scrollDown();
        tc->lblAssertEqUInt("4L.0", 0, this->pane->getSelectedRowRelY());

        this->pane->scrollDown();
        this->pane->scrollDown();
        tc->lblAssertEqUInt("4L.1", 0, this->pane->getSelectedRowRelY());

        this->pane->scrollUp();
        tc->lblAssertEqUInt("4L.2", 0, this->pane->getSelectedRowRelY());

        this->pane->scrollUp();
        this->pane->scrollUp();
        tc->lblAssertEqUInt("4L.3", 0, this->pane->getSelectedRowRelY());
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

TreePaneTestCase2 TreePaneTestCase2::ONLY_VAL;

static const size_t TREE_PANE_UI_SUITE_LEN = 2;
static unit::TestTree *TREE_PANE_UI_SUITE_TESTS[TREE_PANE_UI_SUITE_LEN] = {
    TreePaneTestCase1::ONLY,
    TreePaneTestCase2::ONLY,
};

static unit::TestSuite TREE_PANE_UI_SUITE_VAL("Tree Pane UI Suite", 
        TREE_PANE_UI_SUITE_TESTS, TREE_PANE_UI_SUITE_LEN);

unit::TestTree * const gui::TREE_PANE_UI_SUITE = &TREE_PANE_UI_SUITE_VAL;

static const size_t TREE_PANE_SUITE_LEN = 2;
static unit::TestTree *TREE_PANE_SUITE_TESTS[TREE_PANE_SUITE_LEN] = {
    gui::TREE_PANE_NODE_SUITE,
    gui::TREE_PANE_UI_SUITE,
};

static unit::TestSuite TREE_PANE_SUITE_VAL("Tree Pane Suite",
        TREE_PANE_SUITE_TESTS, TREE_PANE_SUITE_LEN);

unit::TestTree * const gui::TREE_PANE_SUITE = &TREE_PANE_SUITE_VAL;

static const gui::tree_pane_info_t TPANE_INFO = {
    .scrollBarWidth = 8,

    .scrollBarFGColor = 24, 
    .scrollBarBGColor = 223,

    .rowWidth = 40,
    .height = 160,

    .lblVertSpace = 2,
    .lblWidthScale = 1,
    .lblHeightScale = 2,

    .paneBGColor = 247,
    .selRowBGColor = 223,
    .tabWidth = 16,
};

void gui::TreePaneGUITest() {
    core::KeyManager *km = new core::KeyManager(1);
    km->setRepeatDelay(5);
    km->setFocusedKeys(
            (core::cxx_key_t[]){
            core::CXX_KEY_Clear, core::CXX_KEY_8, 
            core::CXX_KEY_5, core::CXX_KEY_Enter}, 4);

    TreePaneNode<LetterState> *root = createTestTree(1, 
            "abA*2lstK3M*1P*2nD*1plQ1S*2Z*3");
    TreePane<LetterState> *pane = 
        new TreePane<LetterState>(1, &TPANE_INFO, root);

    pane->focus();

    gfx_Begin();
    gfx_SetDrawBuffer();

    while (!km->isKeyDown(core::CXX_KEY_Clear)) {
        km->scanFocusedKeys();

        if (km->isKeyPressed(core::CXX_KEY_8)) {
            pane->scrollUp();
        } 
        
        if (km->isKeyPressed(core::CXX_KEY_5)) {
            pane->scrollDown();
        } 
        
        if (km->isKeyPressed(core::CXX_KEY_Enter)) {
            pane->toggle();
        }

        pane->render(50, (GFX_LCD_HEIGHT - 160) / 2);

        gfx_BlitBuffer();
        delay(100);    
    }

    gfx_End();

    delete pane;
    delete root;
    delete km;

    core::MemoryTracker::ONLY->checkMemLeaks();
}
