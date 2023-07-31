
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

    core::CoreList<int> *cl = new core::CoreList<int>(core::CXX_FREE_CHNL_START);

    cl->add(1);
    cl->add(4);
    cl->add(5);

    char buff[30];
    for (int i = 0; i < cl->getLen(); i++) {
        sprintf(buff, "%d : %d", i, cl->get(i));

        os_PutStrFull(buff);
        os_NewLine();
    }

    sprintf(buff, "Cap : %u", cl->getCap());

    os_PutStrFull(buff);
    os_NewLine();

    delete cl;

    core::waitClear();

    core::checkMemLeaks();

    return 0;
}
