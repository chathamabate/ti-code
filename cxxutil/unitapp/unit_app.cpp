#include "./unit_app.h"

#include <cxxutil/app/app.h>

#include "cxxutil/core/input.h"
#include "cxxutil/gui/centered_pane.h"
#include "cxxutil/gui/scroll_text_pane.h"
#include "cxxutil/gui/text_block.h"
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
static constexpr uint8_t UA_YELLOW     = 229;
static constexpr uint8_t UA_RED        = 232;
static constexpr uint8_t UA_PURPLE     = 88;

// NOTE: this needs to be the index of a color
// not used above.
static constexpr uint8_t UA_TRANSPARENT     = 244;

// Delay in ms.
static constexpr uint16_t UA_DELAY          = 50;

static constexpr uint24_t UA_INNER_PANE_WIDTH   = 290;
static constexpr uint8_t  UA_INNER_PANE_HEIGHT  = 196;
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

static const gui::text_color_info_t SUITE_STYLE = {
    .fgColor = UA_BLACK,
    .bgColor = UA_TRANSPARENT,
};

static const gui::text_color_info_t STATUS_STYLES[4] = {
    {.fgColor = UA_BLUE,    .bgColor = UA_TRANSPARENT}, 
    {.fgColor = UA_GREEN,   .bgColor = UA_TRANSPARENT}, 
    {.fgColor = UA_YELLOW,  .bgColor = UA_TRANSPARENT}, 
    {.fgColor = UA_RED,     .bgColor = UA_TRANSPARENT}, 
};

// NOTE: this will be an abstract class as leaf nodes and
// branch nodes will hold different states.
class TestRunState : public core::SafeObject {
private:
    unit::TestTree * const test;
public:
    TestRunState(uint8_t memChnl, unit::TestTree *t)
        : core::SafeObject(memChnl), test(t) {
    }

    TestRunState(unit::TestTree *t) 
        : TestRunState(core::CXX_DEF_CHNL, t) {
    }

    virtual ~TestRunState() {
    }

    virtual void setStatus(TestRunStatus s) = 0;
    
    virtual TestRunStatus getStatus() const  = 0;

    inline unit::TestTree *getTestTree() const {
        return this->test;
    }

    // If a log is currently held, it will be deleted.
    virtual void deleteLog() = 0;

    // If a log is currently held, it will be deleted.
    virtual void resetLog(const gui::scroll_text_pane_info_t *stpi) = 0;

    // NOTE: each test case (when run) will hold
    // a ScrollTextPane. This Pane will store
    // all logged messages during the test run.
    virtual gui::ScrollTextPane *getLog() const = 0;

    inline const char *getLabel() const {
        return test->getName();
    } 

    virtual const gui::text_color_info_t *getLabelInfo() const = 0;

    /*
    inline const gui::text_color_info_t *getLabelInfo() const {
        return &(STATUS_STYLES[(uint8_t)status]);
    }*/
};

class TestRunStateBranch : public TestRunState {
public:
    TestRunStateBranch(uint8_t memChnl, unit::TestTree *t) 
        : TestRunState(memChnl, t) {
    }

    TestRunStateBranch(unit::TestTree *t) 
        : TestRunStateBranch(core::CXX_DEF_CHNL, t) {
    }
    
    virtual ~TestRunStateBranch() {
    }

    virtual inline void setStatus(TestRunStatus s) override {
        (void)s;
    }

    virtual inline TestRunStatus getStatus() const override {
        return TestRunStatus::NOT_RUN;
    }

    virtual inline void deleteLog() override {
    }

    virtual inline void resetLog(const gui::scroll_text_pane_info_t *stpi) override {
        (void)stpi;
    }

    // There is no log for Test Suites!
    // Only logs for Test Cases.
    virtual inline gui::ScrollTextPane *getLog() const override {
        return nullptr;
    }

    virtual const gui::text_color_info_t *getLabelInfo() const override {
        return &SUITE_STYLE;
    }
};

class TestRunStateLeaf : public TestRunState {
private:
    // NOTE: if status is "not run", log will be null!
    gui::ScrollTextPane *log; 
    TestRunStatus status;
public:
    TestRunStateLeaf(uint8_t memChnl, unit::TestTree *t) 
        : TestRunState(memChnl, t) {
        this->log = nullptr;
        this->status = TestRunStatus::NOT_RUN;
    }

    TestRunStateLeaf(unit::TestTree *t) 
        : TestRunStateLeaf(core::CXX_DEF_CHNL, t) {
    }
    
    virtual ~TestRunStateLeaf() {
        if (this->log) {
            delete this->log;
        }
    }

    virtual inline void setStatus(TestRunStatus s) override {
        this->status = s;
    }

    virtual inline TestRunStatus getStatus() const override {
        return this->status;
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

    virtual inline gui::ScrollTextPane *getLog() const override {
        return log;
    }

    virtual inline const gui::text_color_info_t *getLabelInfo() const override {
        return &(STATUS_STYLES[(uint8_t)(this->getStatus())]);
    }
};

static void expandTestInStack(core::CoreList<unit::TestTree *> *stack, 
        unit::TestTree *t) {
    stack->add(t);

    if (t->isCase()) {
        return;
    } 

    unit::TestTree * const *subTests = t->getSubTests();
    size_t subTestsLen = t->getSubTestsLen();

    for (size_t i = 0; i < subTestsLen; i++) {
        expandTestInStack(stack, subTests[i]);
    }
}

static gui::TreePaneNode<TestRunState> *prepareTestTree(uint8_t memChnl, unit::TestTree *t) {
    core::CoreList<unit::TestTree *> *testStack = 
        new core::CoreList<unit::TestTree *>(memChnl);

    expandTestInStack(testStack, t);

    core::CoreList<gui::TreePaneNode<TestRunState> *> *testNodeStack = 
        new core::CoreList<gui::TreePaneNode<TestRunState> *>(memChnl);

    while (testStack->getLen() > 0) {
        unit::TestTree *test = testStack->pop();

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



// -----------------------------------------------------
// -----------------------------------------------------
// BELOW LIES ALL APPLICATION STATES FOR THE UNIT APP!
// -----------------------------------------------------
// -----------------------------------------------------



class UnitAppGlobalState;
class UnitAppLeafTestMonitor;
class UnitAppLeafRunState;
class UnitAppBranchRunState;
class UnitAppLogViewState;
class UnitAppNavState;


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

static const  gui::scroll_text_pane_info_t UA_STPI = {
    .height = UA_INNER_PANE_HEIGHT,
    .lineWidth = UA_INNER_PANE_WIDTH - UA_SCROLL_BAR_WIDTH,
    .scrollBarWidth = UA_SCROLL_BAR_WIDTH,

    .bgColor = UA_WHITE,
    .scrollBarFGColor = UA_PURPLE,
    .scrollBarBGColor = UA_BG_DARK,
    .vertLineSpace = 2,
};

static const gui::text_info_t UA_LOG_HEADER = {
    .widthScale = 2,
    .heightScale = 3,
    .monospace = 0,

    .fgColor = UA_BLUE,
    .bgColor = UA_TRANSPARENT,
};

static const gui::text_info_t UA_LOG_BLACK = {
    .widthScale = 1,
    .heightScale = 2,
    .monospace = 0,

    .fgColor = UA_BLACK,
    .bgColor = UA_TRANSPARENT,
};

static const gui::text_info_t UA_LOG_INFO = {
    .widthScale = 1,
    .heightScale = 2,
    .monospace = 0,

    .fgColor = UA_BLUE,
    .bgColor = UA_TRANSPARENT,
};

static const gui::text_info_t UA_LOG_WARN = {
    .widthScale = 1,
    .heightScale = 2,
    .monospace = 0,

    .fgColor = UA_YELLOW,
    .bgColor = UA_TRANSPARENT,
};

static const gui::text_info_t UA_LOG_FATAL = {
    .widthScale = 1,
    .heightScale = 2,
    .monospace = 0,

    .fgColor = UA_RED,
    .bgColor = UA_TRANSPARENT,
};


class UnitAppLeafTestMonitor : public unit::TestMonitor {
private:
    TestRunStatus maxStatus;

    // This pane will be rerendered every time we print to the
    // scroll text pane.
    gui::CenteredPane<gui::ScrollTextPane> * const pane;

    virtual void notifyCaseStart(unit::TestCase *c) override {
        (void)c;

        this->pane->render(0, 0);

        gfx_SwapDraw();
        gfx_Wait();
    }

    virtual void log(unit::log_level_t level, const char *msg) override {
        gui::ScrollTextPane * const innerLog = this->pane->getInnerPane();

        switch (level) {
        case unit::INFO:
            innerLog->log(&UA_LOG_INFO, msg);
            break;
        case unit::WARN:
            innerLog->log(&UA_LOG_WARN, msg);

            if (this->maxStatus < TestRunStatus::WARN) {
                this->maxStatus = TestRunStatus::WARN;
            }

            break;
        case unit::FATAL:
            innerLog->log(&UA_LOG_FATAL, msg);

            this->maxStatus = TestRunStatus::FAILURE;

            break;
        }

        this->pane->render(0, 0);

        gfx_SwapDraw();
        gfx_Wait();
    }
public:
    UnitAppLeafTestMonitor(gui::CenteredPane<gui::ScrollTextPane> *p) 
        : unit::TestMonitor(), pane(p) {
        this->maxStatus = TestRunStatus::SUCCESS;
    }

    virtual ~UnitAppLeafTestMonitor() {
    }

    inline TestRunStatus getMaxStatus() const {
        return this->maxStatus;
    }
};



// NOTE: The two states for running a test will actually both be run from inside
// the nav pane state. Thus they will always return null.

// This state is for running a singular test case.
class UnitAppLeafRunState : public app::AppState<UnitAppGlobalState> {
private:
    // Remember, node already has a scroll text pane!
    gui::TreePaneNode<TestRunState> * const node;

    gui::CenteredPane<gui::ScrollTextPane> *pane;
    UnitAppLeafTestMonitor *monitor;

    virtual void init() override {
        this->node->getState()->resetLog(&UA_STPI);
        unit::TestTree *t = this->node->getState()->getTestTree();

        this->pane = new gui::CenteredPane<gui::ScrollTextPane>(
                this->getChnl(), &UA_CPI, t->getName(), this->node->getState()->getLog());

        this->monitor = new UnitAppLeafTestMonitor(this->pane);

        // Make sure the user know they cannot scroll.
        this->pane->unFocus();
    }

    virtual AppState<UnitAppGlobalState> *run() override {
        this->node->getState()->getTestTree()->run(this->monitor);

        this->node->getState()->setStatus(this->monitor->getMaxStatus());

        return nullptr;
    }

    virtual void finally() override {
        delete this->monitor;
        delete this->pane;

        // Remember the scroll text pane will persist.
    }

public: 
    UnitAppLeafRunState(uint8_t memChnl, UnitAppGlobalState *gs, 
            gui::TreePaneNode<TestRunState> *n) 
        : AppState<UnitAppGlobalState>(memChnl, gs),
        node(n) {
    }

    UnitAppLeafRunState(UnitAppGlobalState *gs,
            gui::TreePaneNode<TestRunState> *n) 
        : UnitAppLeafRunState(core::CXX_DEF_CHNL, gs, n) {
    }

    virtual ~UnitAppLeafRunState() {
    }
};


// This state is for running a test suite.
class UnitAppBranchRunState : public app::AppState<UnitAppGlobalState> {
private:
    gui::TreePaneNode<TestRunState> * const node;


    virtual void init() override {
    }

    virtual AppState<UnitAppGlobalState> *run() override {
        gui::TreePaneNode<TestRunState> * const * children = this->node->getChildren();
        size_t len = this->node->getChildrenLen();

        for (size_t i = 0; i < len; i++) {
            gui::TreePaneNode<TestRunState> *child = children[i];

            app::AppState<UnitAppGlobalState> *childAppState;

            if (child->isBranch()) {
                childAppState = new UnitAppBranchRunState(this->getChnl(), this->getGlobalState(), child); 
            } else {
                childAppState = new UnitAppLeafRunState(this->getChnl(), this->getGlobalState(), child);
            }

            // run App States in a recursive format from here!
            childAppState->complete();

            delete childAppState;
        }

        return nullptr;
    }

    virtual void finally() override {
    }

public: 
    UnitAppBranchRunState(uint8_t memChnl, UnitAppGlobalState *gs, 
            gui::TreePaneNode<TestRunState> *n) 
        : AppState<UnitAppGlobalState>(memChnl, gs),
        node(n) {
    }

    UnitAppBranchRunState(UnitAppGlobalState *gs,
            gui::TreePaneNode<TestRunState> *n) 
        : UnitAppBranchRunState(core::CXX_DEF_CHNL, gs, n) {
    }

    virtual ~UnitAppBranchRunState() {
    }
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

class UnitAppLogViewState :
    public app::LoopState<UnitAppGlobalState> {
private:
    const char * const title;
    gui::ScrollTextPane * const scrollPane;

    gui::CenteredPane<gui::ScrollTextPane> *pane;

    virtual void init() override {
        this->pane = new gui::CenteredPane<gui::ScrollTextPane>(
                this->getChnl(), &UA_CPI, 
                this->title, this->scrollPane);

        this->scrollPane->focus();
    }

    virtual void update() override {
        core::KeyManager *km = this->getGlobalState()->getKM(); 

        km->scanFocusedKeys();

        if (km->isKeyPressed(core::CXX_KEY_8) || 
                km->isKeyPressed(core::CXX_KEY_Up)) {
            this->pane->getInnerPane()->scrollUp();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_5) || 
                km->isKeyPressed(core::CXX_KEY_Down)) {
            this->pane->getInnerPane()->scrollDown();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_Clear)) {
            this->requestExit(nullptr); 
        }
    }

    virtual void render() override {
        this->pane->render(0, 0);

        gfx_SwapDraw();
        gfx_Wait();
    }

    virtual void finally() override {
        this->scrollPane->unFocus();

        delete this->pane;
    }

public:
    UnitAppLogViewState(uint8_t memChnl, UnitAppGlobalState *gs, 
            const char *t, gui::ScrollTextPane *sp) 
        : LoopState(memChnl, gs, UA_DELAY), title(t), scrollPane(sp) {
    }

    UnitAppLogViewState(UnitAppGlobalState *gs, 
            const char *t, gui::ScrollTextPane *sp) 
        : UnitAppLogViewState(core::CXX_DEF_CHNL, gs, t, sp) {
    }

    virtual ~UnitAppLogViewState() {
    }
};


class UnitAppNavState : 
    public app::LoopState<UnitAppGlobalState> {

private:
    gui::TreePane<TestRunState> *navTree;
    gui::CenteredPane<gui::TreePane<TestRunState>> *navPane;

    gui::ScrollTextPane *helpLog;

    virtual void init() override {
        this->navTree = new gui::TreePane<TestRunState>(
                this->getChnl(), &UA_NAV_TPI, 
                this->getGlobalState()->getTestNodeRoot());

        this->navTree->focus();

        this->navPane = new gui::CenteredPane<gui::TreePane<TestRunState>>(
                this->getChnl(), &UA_CPI, "TI Unit  -  Press 0 for help", this->navTree);

        this->helpLog = new gui::ScrollTextPane(this->getChnl(), &UA_STPI);

        // NOTE: this is kinda a waste of memory, but it's honestly ok.
        this->helpLog->log(&UA_LOG_INFO,    "===================");
        this->helpLog->log(&UA_LOG_HEADER,  "- TI Unit -");
        this->helpLog->log(&UA_LOG_INFO,    "===================");
        this->helpLog->log(&UA_LOG_INFO,    "Controls :");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use Up/Down or 8/5 to scroll up and down.");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use Enter to expand a test suite.");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use 9 to run a test case or suite.");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use 6 to view a test case log.");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use 3 to delete the test log(s) of a case or suite.");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use 0 to view this page.");
        this->helpLog->log(&UA_LOG_BLACK,   "* Use Clear to return to the main page or exit.");
        this->helpLog->log(&UA_LOG_INFO,    "================");
        this->helpLog->log(&UA_LOG_INFO,    "About :");
        this->helpLog->log(&UA_LOG_BLACK,   "This project was created by Chatham Abate." );

        this->helpLog->gotoTop();

        this->getGlobalState()->getKM()->setFocusedKeys(
                (core::cxx_key_t[]){
                    core::CXX_KEY_Clear, 
                    core::CXX_KEY_Up,
                    core::CXX_KEY_Down,
                    core::CXX_KEY_8,
                    core::CXX_KEY_5,
                    core::CXX_KEY_Enter,

                    core::CXX_KEY_9,
                    core::CXX_KEY_6,
                    core::CXX_KEY_3,

                    core::CXX_KEY_0
                }, 10);
    }

    virtual void update() override {
        core::KeyManager *km  = this->getGlobalState()->getKM();

        km->scanFocusedKeys();
        
        if (km->isKeyPressed(core::CXX_KEY_Clear)) {
            this->requestExit(nullptr);
        } else if (km->isKeyPressed(core::CXX_KEY_Up) || 
                km->isKeyPressed(core::CXX_KEY_8)) {
            this->navTree->scrollUp();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_Down) || 
                km->isKeyPressed(core::CXX_KEY_5)) {
            this->navTree->scrollDown();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_Enter)) {
            this->navTree->toggle();
            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_9)) {
            // Time to run our test!

            gui::TreePaneNode<TestRunState> *selNode = 
                this->navTree->getSelectedNode();

            app::AppState<UnitAppGlobalState> *nodeRunState;

            if (selNode->isBranch()) {
                nodeRunState = new UnitAppBranchRunState(
                        this->getChnl(), this->getGlobalState(), selNode);
            } else {
                nodeRunState = new UnitAppLeafRunState(
                        this->getChnl(), this->getGlobalState(), selNode);
            }

            nodeRunState->complete();
            delete nodeRunState;

            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_6)) {
            gui::TreePaneNode<TestRunState> *selNode = 
                this->navTree->getSelectedNode();

            if (selNode->isLeaf() && selNode->getState()->getLog()) {
                UnitAppLogViewState *viewState = new UnitAppLogViewState(
                        this->getChnl(), this->getGlobalState(), 
                        selNode->getState()->getTestTree()->getName(), selNode->getState()->getLog());

                viewState->complete();

                delete viewState;

                this->requestRedraw();
            }
        } else if (km->isKeyPressed(core::CXX_KEY_3)) {
            gui::TreePaneNode<TestRunState> *selNode = 
                this->navTree->getSelectedNode();
            
            // Kinda rushed stack algo here.
            // This would be a nice place for a foreach function.
            
            core::CoreList<gui::TreePaneNode<TestRunState> *> *s = 
                new core::CoreList<gui::TreePaneNode<TestRunState> *>(this->getChnl());

            s->add(selNode);

            while (s->getLen() > 0) {
                gui::TreePaneNode<TestRunState> *node = s->pop();

                if (node->isLeaf()) {
                    node->getState()->deleteLog();
                    node->getState()->setStatus(TestRunStatus::NOT_RUN);
                } else {
                    gui::TreePaneNode<TestRunState> * const *children = 
                        node->getChildren();

                    size_t childrenLen = node->getChildrenLen();

                    for (size_t i = 0; i < childrenLen; i++) {
                        s->add(children[i]);
                    }
                }
            }

            delete s;

            this->requestRedraw();
        } else if (km->isKeyPressed(core::CXX_KEY_0)) {
            UnitAppLogViewState *helpState = new UnitAppLogViewState(
                    this->getChnl(), this->getGlobalState(), 
                    "TI Unit  -  Help Page", this->helpLog);

            helpState->complete();

            delete helpState;

            this->requestRedraw();
        }
    }

    virtual void render() override {
        this->navPane->render(0, 0);

        gfx_SwapDraw();
        gfx_Wait();
    }

    virtual void finally() override {
        delete this->navPane;
        delete this->navTree;

        delete this->helpLog;
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


void cxxutil::unitapp::runUnitApp(unit::TestTree *testTree) {
    core::MemoryExitRoutine *oldMer = core::MemoryTracker::ONLY->getMER();

    // This will contain the test logs.
    UnitAppGlobalState *gs = new UnitAppGlobalState(core::CXX_TEST_CHNL, testTree);
    UnitAppNavState *initState = new UnitAppNavState(1, gs);

    gfx_Begin();
    core::MemoryTracker::ONLY->setMER(core::GraphicsMemoryExitRoutine::ONLY);

    gfx_SetDrawBuffer();

    gfx_SetTransparentColor(UA_TRANSPARENT);
    gfx_SetTextTransparentColor(UA_TRANSPARENT);

    app::AppState<UnitAppGlobalState>::runApp(initState);

    gfx_End();

    core::MemoryTracker::ONLY->setMER(oldMer);

    delete initState;
    delete gs;
}
