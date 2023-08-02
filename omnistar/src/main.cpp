
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

static void test1(unit::TestContext *tc) {
    tc->info("Message1");
    tc->assertTrue(true);

    // This all actually seems to work!
    tc->lblAssertEqInt("lbl1", 1, 2);
    tc->info("Message2");
}

static const unit::unit_test_t TEST1 = {
    .name = "My Test 1",
    .test = test1,
};


int main(void) {    
    os_ClrHome();

    const unit::TestRun *tr = unit::runUnitTest(&TEST1);

    const core::CoreList<unit::TestLogLine *> *logs = tr->getLogs(); 
    const int len = logs->getLen();

    for (int i = 0; i < len; i++) {
        os_PutStrFull(logs->get(i)->getMsg());
        os_NewLine();
    }

    core::waitClear();

    delete tr;
    
    core::checkMemLeaks();

    return 0;
}
