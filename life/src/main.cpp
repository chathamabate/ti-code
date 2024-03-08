#include <cxxutil/core/input.h>
#include <ti/screen.h>


using namespace cxxutil;

int main(void) {
    // Ok, nice, this works!
    os_ClrHome(); 
    os_PutStrLine("Hello World");

    while (!os_GetCSC());

}
