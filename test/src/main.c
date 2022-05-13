// #define HAS_PRINTF YES

#include <tice.h>
#include <graphx.h>

#include <stdio.h>
#include <stdlib.h>

void test_run(void);

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
    gfx_SetTextScale(4, 4);

    test_run();


    /* Waits for a key */
    while (!os_GetCSC());

    /* End graphics drawing */
    gfx_End();

    return 0;
}

void test_run(void) {
    uint8_t x, y;

    x = 10; y = 12;
    char buff[100];
    
    switch (8)
    {
    case 8:
        sprintf(buff, "%d %d", x, y);
        gfx_PrintStringXY(buff, 10, 10);
        /* code */
        return;
    }

    x++;
    return;
}