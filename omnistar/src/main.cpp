
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
#include <cxxutil/mem.h>
#include <cxxutil/input.h>

using namespace cxxutil;

constexpr uint8_t KEYS_LEN = 5;
const cxx_key_t KEYS[KEYS_LEN] = {
    CXX_KEY_Clear,

    CXX_KEY_7,
    CXX_KEY_4,
    CXX_KEY_1,
    CXX_KEY_0,
};

class MyObject : public SafeObject {
public:
    int x[10];
    MyObject() : SafeObject(3) {
        this->x[0] = 10;
    }
};

int main(void) {
    KeyManager *km = new KeyManager(CXX_TEST_CHNL);

    km->setRepeatDelay(10);
    km->setFocusedKeys(KEYS, KEYS_LEN);

    os_ClrHome();

    while (1) {
        delay(50);
        km->scanFocusedKeys();

        if (km->isKeyPressed(CXX_KEY_0)) {
            os_PutStrFull("0 Pressed.");
            os_NewLine();
        }

        if (km->isKeyPressed(CXX_KEY_1)) {
            os_PutStrFull("Disabling");
            os_NewLine();

            km->setFocusedKeys(KEYS, 1);
        }

        if (km->isKeyPressed(CXX_KEY_Clear)) {
            break;
        }
    }

    delete km;

#ifdef CXX_MEM_CHECKS
    checkMemLeaks();
#endif

    return 0;
}
