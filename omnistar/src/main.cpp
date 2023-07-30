
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
#include <cxxutil/core/mem.h>
#include <cxxutil/core/input.h>

using namespace cxxutil;

constexpr uint8_t KEYS_LEN = 5;
const core::cxx_key_t KEYS[KEYS_LEN] = {
    core::CXX_KEY_Clear,

    core::CXX_KEY_7,
    core::CXX_KEY_4,
    core::CXX_KEY_1,
    core::CXX_KEY_0,
};

class MyObject : public core::SafeObject {
public:
    int x[10];
    MyObject() : SafeObject(3) {
        this->x[0] = 10;
    }
};

int main(void) {    


    core::checkMemLeaks();

    return 0;
}
