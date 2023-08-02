
#include "keypadc.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstddef>
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

    const size_t destSize = 10;
    char dest[destSize];

    const size_t strsSize = 6;
    const char *strs[strsSize] = {
        "hello", " ", "", "m", "ygood", ""
    };

    size_t finalLen = core::multiStrCatSafe(dest, 0, 
            destSize, strsSize, strs);

    os_PutStrFull(dest);
    os_NewLine();

    char lenBuf[50];
    sprintf(lenBuf, "Len: %u (%p)", finalLen, dest);

    os_PutStrFull(lenBuf);
    os_NewLine();

    core::waitClear();
    
    core::checkMemLeaks();

    return 0;
}
