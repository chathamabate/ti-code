
#include "cxxutil/core/mem.h"
#include "ti/getcsc.h"
#include "ti/getkey.h"
#include <ti/screen.h>
#include <cxxutil/core/input.h>

using namespace cxxutil;

int main(void) {
    os_ClrHome();
    os_PutStrFull("Hello World!");

    // Basic starting point done!

    core:: KeyManager * const km = new core::KeyManager(1);

    const core::cxx_key_t KEYS[1] = {
        core::CXX_KEY_0
    };

    km->setFocusedKeys(KEYS, 1);

    while (1) {
        km->scanFocusedKeys();

        if (km->isKeyDown(core::CXX_KEY_0)) {
            break;
        }
    }

    delete km;

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}

