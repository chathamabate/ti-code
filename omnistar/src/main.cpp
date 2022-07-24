
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <tice.h>
#include <stdio.h>

#include <cxxutil/test.h>

using namespace cxxutil;

int main(void) {


    Student s(10);

    os_ClrHome(); 
    s.SayAge();

    while (!os_GetCSC());

    return 0;
}
