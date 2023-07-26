
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

    incrMemChnl(1);
    incrMemChnl(1);
    decrMemChnl(1);

    decrMemChnl(200);


    return 0;
}
