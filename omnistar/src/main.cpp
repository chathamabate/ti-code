
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

}

static const unit::unit_test_t TEST1 = {
    .name = "My Test 1",
    .test = test1,
};


int main(void) {    
    os_ClrHome();

    const unit::TestRun *tr = unit::runUnitTest(&TEST1);

    core::waitClear();
    
    core::checkMemLeaks();

    return 0;
}
