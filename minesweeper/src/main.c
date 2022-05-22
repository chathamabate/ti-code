#include <stdio.h>
#include <tice.h>

// Game states and menus could be improved from snake...
// Archiving will probs be the same TBH...
// Sprite rendering will need to be researched...

int main(void) {
    os_PutStrFull("Hello World!");
    while (os_GetCSC() != sk_Clear);

    return 0;
}