
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
#include <cxxutil/core/data.h>

using namespace cxxutil;


int main(void) {    
    os_ClrHome();
    int y = 0;
    int *x = &y;

    if (x) {
        os_PutStrFull("This, didn't work");
        os_NewLine();
    }

    core::waitClear();
    

    core::checkMemLeaks();

    return 0;
}
