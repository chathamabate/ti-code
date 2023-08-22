#include "./unit_app.h"
#include "cxxutil/gui/tree_pane.h"
#include "cxxutil/unit/unit.h"
#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>

// We are including these tests to test the app itself.
#include <cxxutil/gui/test/tree_pane.h>

using namespace cxxutil;



// This is the state used in each of the tree pane nodes
// when navigating through tests.
//
//
// Each node will hold one of these codes.
enum class TestRunStatus : uint8_t {
    NOT_RUN = 0,

    // If no fatal error messages and no warning messages
    // were logged, this status will be used.
    SUCCESS = 1,

    // If no fatal error messages were logged and at
    // least one warn message was loggeed, this status
    // will be used.
    WARN = 2,

    // If at least one fatal error message was logged
    // while running the test tree, this status will 
    // be used.
    FAILURE = 3,
};

const gui::text_color_info_t STATUS_STYLES[4] = {
    {.fgColor = 0, .bgColor = 255}, 
    {.fgColor = 0, .bgColor = 255}, 
    {.fgColor = 0, .bgColor = 255}, 
    {.fgColor = 0, .bgColor = 255}, 
};

// NOTE: this will be an abstract class as leaf nodes and
// branch nodes will hold different states.
class TestRunState : public core::SafeObject {
private:
    const unit::TestTree * const test;
    TestRunStatus status;
public:
    TestRunState(uint8_t memChnl, const unit::TestTree *t)
        : core::SafeObject(memChnl), test(t) {
        this->status = TestRunStatus::NOT_RUN; 
    }

    TestRunState(const unit::TestTree *t) 
        : TestRunState(core::CXX_DEF_CHNL, t) {
    }

    virtual ~TestRunState() {
    }

    inline void setStatus(TestRunStatus s) {
        this->status = s;
    }
    
    inline TestRunStatus getStatus() {
        return this->status;
    }

    virtual void setLog(gui::ScrollTextPane *log) = 0;

    // NOTE: each test case (when run) will hold
    // a ScrollTextPane. This Pane will store
    // all logged messages during the test run.
    virtual gui::ScrollTextPane *getLog() = 0;

    inline const char *getLabel() const {
        return test->getName();
    } 

    inline const gui::text_color_info_t *getLabelInfo() const {
        return &(STATUS_STYLES[(uint8_t)status]);
    }
};

class TestRunStateBranch : public TestRunState {
public:
    TestRunStateBranch(uint8_t memChnl, const unit::TestTree *t) 
        : TestRunState(memChnl, t) {
    }

    TestRunStateBranch(const unit::TestTree *t) 
        : TestRunStateBranch(core::CXX_DEF_CHNL, t) {
    }
    
    virtual ~TestRunStateBranch() {
    }

    virtual inline void setLog(gui::ScrollTextPane *log) override {
        (void)log;
    }

    // There is no log for Test Suites!
    // Only logs for Test Cases.
    virtual inline gui::ScrollTextPane *getLog() override {
        return nullptr;
    }
};

class TestRunStateLeaf : public TestRunState {
private:
    // NOTE: if status is "not run", log will be null!
    gui::ScrollTextPane *log; 
public:
    TestRunStateLeaf(uint8_t memChnl, const unit::TestTree *t) 
        : TestRunState(memChnl, t) {
        this->log = nullptr;
    }

    TestRunStateLeaf(const unit::TestTree *t) 
        : TestRunStateLeaf(core::CXX_DEF_CHNL, t) {
    }
    
    virtual ~TestRunStateLeaf() {
    }

    virtual inline void setLog(gui::ScrollTextPane *log) override {
        this->log = log;
    }

    virtual inline gui::ScrollTextPane *getLog() override {
        return log;
    }
};

static void expandTestInStack(core::CoreList<const unit::TestTree *> *stack, 
        const unit::TestTree *t) {
    stack->add(t);

    if (t->isCase()) {
        return;
    } 

    const unit::TestTree * const *subTests = t->getSubTests();
    size_t subTestsLen = t->getSubTestsLen();

    for (size_t i = 0; i < subTestsLen; i++) {
        expandTestInStack(stack, subTests[i]);
    }
}

static gui::TreePaneNode<TestRunState> *prepareTestTree(uint8_t memChnl, const unit::TestTree *t) {
    core::CoreList<const unit::TestTree *> *testStack = 
        new core::CoreList<const unit::TestTree *>(memChnl);

    expandTestInStack(testStack, t);

    core::CoreList<gui::TreePaneNode<TestRunState> *> *testNodeStack = 
        new core::CoreList<gui::TreePaneNode<TestRunState> *>(memChnl);

    while (testStack->getLen() > 0) {
        const unit::TestTree *test = testStack->pop();

        if (test->isCase()) {
            TestRunStateLeaf *caseState = new TestRunStateLeaf(memChnl, test);
            gui::TreePaneLeaf<TestRunState> *leafNode = 
                new gui::TreePaneLeaf<TestRunState>(memChnl, caseState);
            
            testNodeStack->add(leafNode);
        } else {
            TestRunStateBranch *branchState = 
                new TestRunStateBranch(memChnl, test);

            const size_t subTestsLen = test->getSubTestsLen();

            core::SafeArray<gui::TreePaneNode<TestRunState> *> *childNodes = 
                new core::SafeArray<gui::TreePaneNode<TestRunState> *>(memChnl, subTestsLen);

            for (size_t i = 0; i < subTestsLen; i++) {
                childNodes->set(i, testNodeStack->pop());
            }

            gui::TreePaneBranch<TestRunState> *branchNode =
                new gui::TreePaneBranch<TestRunState>(branchState, childNodes);

            testNodeStack->add(branchNode);
        }
    }

    gui::TreePaneNode<TestRunState> *root = testNodeStack->pop();
    root->declareRoot();

    delete testNodeStack;
    delete testStack;

    return root;
}

static const gui::tree_pane_info_t TPANE_INFO = {
    .scrollBarWidth = 8,

    .scrollBarFGColor = 24, 
    .scrollBarBGColor = 223,

    .rowWidth = 200,
    .height = 160,

    .lblVertSpace = 2,
    .lblWidthScale = 1,
    .lblHeightScale = 2,

    .paneBGColor = 247,
    .selRowBGColor = 223,
    .tabWidth = 16,
};

void cxxutil::unitapp::runTestGUITest() {
    gui::TreePaneNode<TestRunState> *root = 
        prepareTestTree(1, gui::TREE_PANE_SUITE); 

    core::KeyManager *km = new core::KeyManager(1);
    km->setRepeatDelay(5);
    km->setFocusedKeys(
            (core::cxx_key_t[]){
            core::CXX_KEY_Clear, core::CXX_KEY_8, 
            core::CXX_KEY_5, core::CXX_KEY_Enter}, 4);

    gui::TreePane<TestRunState> *pane 
        = new gui::TreePane<TestRunState>(1, &TPANE_INFO, root);

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
