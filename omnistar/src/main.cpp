
#include "keypadc.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <new>
#include <graphx.h>

#include <cutil/keys.h>
#include <cxxutil/core/mem.h>
#include <cxxutil/core/input.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>

using namespace cxxutil;

// This is like a massive declaration for a single
// test...

class MyTestCase : public unit::TestCase {
private:
    static MyTestCase ONLY_VAL;

    MyTestCase() : TestCase("MyTest") {}
    // Pointers to dynamic memory...
public:
    static constexpr unit::TestCase * ONLY = &ONLY_VAL;

    virtual void attempt(unit::TestContext *tc) override {
        // Test Code....
    }

    virtual void finally() override {
        // Clean Up Code...
    }
};

MyTestCase MyTestCase::ONLY_VAL;

int main(void) {    
    os_ClrHome();

    const unit::TestRun *tr = unit::runUnitTest(MyTestCase::ONLY);

    const core::CoreList<unit::TestLogLine *> *logs = tr->getLogs(); 
    const int len = logs->getLen();

    for (int i = 0; i < len; i++) {
        os_PutStrFull(logs->get(i)->getMsg());
        os_NewLine();
    }

    if (tr->getMemLeak()) {
        os_PutStrFull("Mem Leaks Found");
        os_NewLine();
    }

    core::waitClear();

    delete tr;
    
    core::checkMemLeaks();

    return 0;
}
