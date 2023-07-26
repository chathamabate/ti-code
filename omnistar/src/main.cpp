
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

class MyObject : public SafeObject {
public:
    int x[10];
    MyObject() : SafeObject(3) {
        this->x[0] = 10;
    }
};

int main(void) {
    SafeArray<MyObject> *arr = new SafeArray<MyObject>(1, 10);

#ifdef CXX_MEM_CHECKS
    checkMemLeaks();
#endif

    return 0;
}
