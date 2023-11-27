
#include "cxxutil/core/mem.h"
#include "cxxutil/unitapp/unit_app.h"
#include "ti/getcsc.h"
#include "ti/getkey.h"
#include <ti/screen.h>
#include <cxxutil/core/input.h>

#include <cxxutil/unit/unit.h>
#include <cxxutil/data/test/bits.h>

using namespace cxxutil;

int main(void) {

    unitapp::runUnitApp(data::BIT_VECTOR_SUITE);

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}

