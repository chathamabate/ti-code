
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

int main(void) {
    // os_ClrHome();
    os_PutStrFull("Starting Main!");
    os_NewLine();


    while (!kb_IsDown(kb_KeyClear)) {
        delay(50);
        kb_Scan();
    }

    return 0;
}
