
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

using namespace cxxutil;

class X : public SafeObject {
    public:
    int arr[20];
    X() : SafeObject(4) {

    }
};

int main(void) {
    // MemoryTracker::getInstance()
    //    ->setMemoryExitRoutine(BasicMemoryExitRoutine::getInstance());

    X *ptr;

    while (true) {
        ptr = new X();
    }

    // Exceptions are disabled!
    return 0;
}
