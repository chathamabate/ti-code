#include "./unit_app.h"

#include <cxxutil/app/app.h>

#include "cxxutil/core/input.h"
#include "cxxutil/gui/centered_pane.h"
#include "cxxutil/gui/scroll_text_pane.h"
#include "cxxutil/gui/tree_pane.h"
#include "cxxutil/unit/unit.h"
#include "graphx.h"
#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>

// We are including these tests to test the app itself.
#include <cxxutil/gui/test/tree_pane.h>

using namespace cxxutil;

// Let's define some colors that will be used for this app.
// UA is for UnitApp. These colors were chosen from the default 
// color pallete.
static constexpr uint8_t UA_WHITE      = 255;

static constexpr uint8_t UA_BG_LIGHT    = 222;
static constexpr uint8_t UA_BG_MID      = 191;
static constexpr uint8_t UA_BG_DARK     = 148;

static constexpr uint8_t UA_BLACK      = 0;
static constexpr uint8_t UA_BLUE       = 24;
static constexpr uint8_t UA_GREEN      = 6;
static constexpr uint8_t UA_YELLOW     = 230;
static constexpr uint8_t UA_RED        = 232;
static constexpr uint8_t UA_PURPLE     = 88;

// NOTE: this needs to be the index of a color
// not used above.
static constexpr uint8_t UA_TRANSPARENT     = 244;

// Delay in ms.
static constexpr uint16_t UA_DELAY          = 50;

static constexpr uint24_t UA_INNER_PANE_WIDTH   = 280;
static constexpr uint8_t  UA_INNER_PANE_HEIGHT  = 178;
static constexpr uint8_t  UA_SCROLL_BAR_WIDTH    = 8;


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
    {.fgColor = UA_BLUE,    .bgColor = UA_TRANSPARENT}, 
    {.fgColor = UA_GREEN,   .bgColor = UA_TRANSPARENT}, 
    {.fgColor = UA_YELLOW,  .bgColor = UA_TRANSPARENT}, 
    {.fgColor = UA_RED,     .bgColor = UA_TRANSPARENT}, 
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

    // If a log is currently held, it will be deleted.
    virtual void deleteLog() = 0;

    // If a log is currently held, it will be deleted.
    virtual void resetLog(const gui::scroll_text_pane_info_t *stpi) = 0;

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

    virtual inline void deleteLog() override {
    }

    virtual inline void resetLog(const gui::scroll_text_pane_info_t *stpi) override {
        (void)stpi;
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
        if (this->log) {
            delete this->log;
        }
    }

    virtual inline void deleteLog() override {
        if (this->log) {
            delete this->log;
            this->log = nullptr;
        }
    }

    virtual inline void resetLog(const gui::scroll_text_pane_info_t *stpi) override {
        if (this->log) {
            delete this->log;
        }

        this->log = new gui::ScrollTextPane(this->getChnl(), stpi);
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

// Now it's time to define the application states.
// What about the key manager???

class UnitAppGlobalState : public core::SafeObject {
private:
    unit::TestTree * const testRoot;
    gui::TreePaneNode<TestRunState> * const testNodeRoot;
    core::KeyManager * const km;

public:
    UnitAppGlobalState(uint8_t memChnl, unit::TestTree *t) 
        : core::SafeObject(memChnl), 
        testRoot(t), 
        testNodeRoot(prepareTestTree(memChnl, t)),
        km(new core::KeyManager(memChnl)) {

        this->km->setRepeatDelay(5);
    }

    UnitAppGlobalState(unit::TestTree *t) 
        : UnitAppGlobalState(core::CXX_DEF_CHNL, t) {
    }

    ~UnitAppGlobalState() {
        delete this->km;
        delete this->testNodeRoot; 
    }

    inline unit::TestTree *getTestRoot() const {
        return this->testRoot;
    }

    inline gui::TreePaneNode<TestRunState> *getTestNodeRoot() const {
        return this->testNodeRoot;
    }

    inline core::KeyManager *getKM() const {
        return this->km;
    }
};

static const gui::centered_pane_info_t  UA_CPI = {
    .titleWidthScale = 1,
    .titleHeightScale = 2,

    .titleFGColor = UA_WHITE,
    .titleBGColor = UA_PURPLE,
    .titlePadding = 2,

    .borderBGColor = UA_BG_LIGHT,

    .width = GFX_LCD_WIDTH,
    .height = GFX_LCD_HEIGHT,
};

static const gui::tree_pane_info_t UA_NAV_TPI = {
    .scrollBarWidth = UA_SCROLL_BAR_WIDTH,
    .scrollBarFGColor = UA_PURPLE,
    .scrollBarBGColor = UA_BG_DARK,

    .rowWidth = UA_INNER_PANE_WIDTH - UA_SCROLL_BAR_WIDTH,
    .height = UA_INNER_PANE_HEIGHT,

    .lblVertSpace = 2,

    .lblWidthScale = 1,
    .lblHeightScale = 2,

    .paneBGColor = UA_WHITE,
    .selRowBGColor = UA_BG_MID,

    .tabWidth = 16,

    .transparentColor = UA_TRANSPARENT,
};

class UnitAppNavState : 
    public app::LoopState<UnitAppGlobalState> {

private:
    gui::TreePane<TestRunState> *treePane;
    gui::CenteredPane<gui::TreePane<TestRunState>> *centeredPane;

    virtual void init() override {
        this->treePane = new gui::TreePane<TestRunState>(
                this->getChnl(), &UA_NAV_TPI, 
                this->getGlobalState()->getTestNodeRoot());

        this->treePane->focus();

        this->centeredPane = new gui::CenteredPane<gui::TreePane<TestRunState>>(
                this->getChnl(), &UA_CPI, "TI Unit  -  Press 0 for help", treePane);

        this->getGlobalState()->getKM()->setFocusedKeys(
                (core::cxx_key_t[]){
                    core::CXX_KEY_Clear, 
                    core::CXX_KEY_Up,
                    core::CXX_KEY_Down,
                    core::CXX_KEY_8,
                    core::CXX_KEY_5,
                    core::CXX_KEY_Enter,
                }, 6);
    }

    virtual void update() override {
        core::KeyManager *km  = this->getGlobalState()->getKM();

        km->scanFocusedKeys();
        
        if (km->isKeyPressed(core::CXX_KEY_Clear)) {
            this->requestExit(nullptr);
        } else if (km->isKeyPressed(core::CXX_KEY_Up) || 
                km->isKeyPressed(core::CXX_KEY_8)) {
            this->treePane->scrollUp();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_Down) || 
                km->isKeyPressed(core::CXX_KEY_5)) {
            this->treePane->scrollDown();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_Enter)) {
            this->treePane->toggle();
            this->requestRedraw();
        }
    }

    virtual void render() override {
        this->centeredPane->render(0, 0);

        gfx_SwapDraw();
        gfx_Wait();
    }

    virtual void finally() override {
        delete this->centeredPane;
        delete this->treePane;
    }

public:
    UnitAppNavState(uint8_t memChnl, UnitAppGlobalState *gs) 
        : LoopState<UnitAppGlobalState>(memChnl, gs, UA_DELAY) {
    }

    UnitAppNavState(UnitAppGlobalState *gs) 
        : UnitAppNavState(core::CXX_DEF_CHNL, gs) {
    }

    virtual ~UnitAppNavState() {
    }
};


void cxxutil::unitapp::runTestGUITest() {
    UnitAppGlobalState *gs = new UnitAppGlobalState(1, gui::TREE_PANE_SUITE);
    UnitAppNavState *initState = new UnitAppNavState(1, gs);

    gfx_Begin();
    gfx_SetDrawBuffer();

    gfx_SetTransparentColor(UA_TRANSPARENT);
    gfx_SetTextTransparentColor(UA_TRANSPARENT);

    app::AppState<UnitAppGlobalState>::runApp(initState);

    gfx_End();

    delete initState;
    delete gs;

    /*
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
    */

    core::MemoryTracker::ONLY->checkMemLeaks();
}
