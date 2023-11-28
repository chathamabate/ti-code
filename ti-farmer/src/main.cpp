
#include "cxxutil/core/mem.h"
#include "cxxutil/unitapp/unit_app.h"
#include "ti/getcsc.h"
#include "ti/getkey.h"
#include <ti/screen.h>
#include <cxxutil/core/input.h>

#include <cxxutil/unit/unit.h>
#include <cxxutil/data/test/bits.h>

#include <cxxutil/data/bits.h>
#include <stdio.h>

using namespace cxxutil;

class X : public core::SafeObject {
private:
    int y;
public:
    X() : X(core::CXX_DEF_CHNL) {}
    X(uint8_t chnl) : core::SafeObject(chnl) {}
};

void innerMain(void) {

    os_ClrHome();

    X x1(2);
    X x2(3);

    x2 = x1;

    char buf[30];
    sprintf(buf, "x1: %u   x2: %u", x1.getChnl(), x2.getChnl());

    os_PutStrFull(buf);

    while (os_GetCSC() != sk_Clear);
}

int main(void) {
    innerMain();

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}

