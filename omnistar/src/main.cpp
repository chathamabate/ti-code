
#include "keypadc.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <new>
#include <graphx.h>

#include <cutil/keys.h>

#include <cxxutil/game.h>
#include <cxxutil/mem.h>
#include <cxxutil/input.h>

using namespace cxxutil;

int main(void) {

#ifdef CXX_MEM_CHECKS
    // Memory Tracker initialization code!
    MemoryTracker::initMemoryTracker();
    MemoryTracker::getInstance()
        ->setMemoryExitRoutine(BasicMemoryExitRoutine::getInstance());
#endif

    MemoryTracker::getInstance()->runMemoryExitRoutine(MemoryExitCode::OUT_OF_MEMORY);

    // Exceptions are disabled!
    return 0;
}
