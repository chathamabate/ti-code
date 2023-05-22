
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

int main(void) {
    MemoryTracker::getInstance()
        ->setMemoryExitRoutine(BasicMemoryExitRoutine::getInstance());
    os_ClrHome();

    SafeArray<int> *arr1, *arr2;

    arr1 = new SafeArray<int>(5, 20000);
    arr2 = new SafeArray<int>(6, 30000);

    uint16_t i;

    os_PutStrFull("Setting Arr 1"); os_NewLine();
    for (i = 0; i < arr1->getLen(); i++) {
        arr1->set(i, i);
    }

    os_PutStrFull("Setting Arr 2"); os_NewLine();
    for (i = 0; i < arr2->getLen(); i++) {
        arr2->set(i, i);
    }

    os_PutStrFull("Checking Arr 1"); os_NewLine();
    for (i = 0; i < arr1->getLen(); i++) {
        if (arr1->get(i) != i) {
            os_PutStrFull("Mistake 1");
            os_NewLine();
        }
    }

    os_PutStrFull("Checking Arr 2"); os_NewLine();
    for (i = 0; i < arr2->getLen(); i++) {
        if (arr2->get(i) != i) {
            os_PutStrFull("Mistake 2");
            os_NewLine();
        }
    }

    MemoryTracker::getInstance()->printMemChnls();

    delay(3000);

    // Exceptions are disabled!
    return 0;
}
