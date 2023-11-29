
#include "cxxutil/core/mem.h"
#include "cxxutil/unitapp/unit_app.h"
#include "ti/getcsc.h"
#include "ti/getkey.h"
#include <ti/screen.h>
#include <cxxutil/core/input.h>

#include <cxxutil/unit/unit.h>
#include <cxxutil/data/test/bits.h>
#include <cxxutil/data/test/heap.h>
#include <cxxutil/data/heap.h>

#include <cxxutil/data/bits.h>
#include <stdio.h>

using namespace cxxutil;


void innerMain(void) {
    unitapp::runUnitApp(data::HEAP_SUITE);
}

int main(void) {
    innerMain();

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}

