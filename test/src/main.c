// #define HAS_PRINTF YES

#include <tice.h>
#include <graphx.h>

#include <stdio.h>
#include <stdlib.h>

/* Main function, called first */
int main(void)
{
    /* Clear the homescreen */
    // os_ClrHome();

    // char buff[10];
    // sprintf(buff, "%d", sizeof(int)); // ints are 3 bytes because of ez80 architecture!

    // os_PutStrLine(buff);

    gfx_Begin();

    /* Set the text colors */
    gfx_SetTextFGColor(6);
    gfx_SetTextBGColor(255);

    /* Print some scaled text */
    gfx_SetTextScale(1, 1);
    gfx_PrintStringXY("SCALED TXT", 10, 10);

    /* Waits for a key */
    while (!os_GetCSC());

    /* End graphics drawing */
    gfx_End();

    return 0;
}