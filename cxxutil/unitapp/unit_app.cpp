#include "./unit_app.h"
#include <cxxutil/core/mem.h>

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
    {.fgColor = 0, .bgColor = 0}, 
    {.fgColor = 0, .bgColor = 0}, 
    {.fgColor = 0, .bgColor = 0}, 
    {.fgColor = 0, .bgColor = 0}, 
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

static gui::TreePaneNode<TestRunState> *prepareTestTree() {

}
