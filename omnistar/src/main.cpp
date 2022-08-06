
#include "keypadc.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <graphx.h>

#include <cutil/keys.h>
#include <cxxutil/test.h>
#include <cxxutil/game.h>

using namespace cxxutil;

int main(void) {

    // Lambdas Work!!!!
    // auto f = [] (int a, int b) {return a + b;};
    
    // One or two more fields per shape not a big deal...
    Shape *shapes[] = {
        new Rectangle(3, 5),
        new Square(2)
    };
    
    os_ClrHomeFull();

    for (int i = 0; i < 2; i++) {
        delete shapes[i];
        os_NewLine();
    }

    while (!os_GetCSC());

    return 0;
}
