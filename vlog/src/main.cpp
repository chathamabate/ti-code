
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

#include "cxxutil/gui/test/centered_pane.h"
#include "cxxutil/unitapp/unit_app.h"

#include <cxxutil/core/mem.h>
#include <cxxutil/core/input.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>

#include <cxxutil/gui/test/text_block.h>
#include <cxxutil/gui/test/tree_pane.h>
#include <cxxutil/gui/test/scroll_text_pane.h>

using namespace cxxutil;

class MyTestCase : public unit::TestCase {
private:
    static MyTestCase ONLY_VAL;
    MyTestCase() : TestCase("MyTest") {}

    // Pointers to dynamic memory...
    
    virtual void attempt(unit::TestContext *tc) override {
        tc->warn("This is a warning");
        // Test Code....
    }

    virtual void finally() override {
        // Clean Up Code...
    }
public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

MyTestCase MyTestCase::ONLY_VAL;


int main(void) {    
    unitapp::runUnitApp(MyTestCase::ONLY);
    
    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
