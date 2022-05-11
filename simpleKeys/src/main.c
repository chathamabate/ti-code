
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>

void PrintCentered(const char *str);

int main(void) {
    kb_key_t key;

    gfx_Begin();

    uint8_t down = 0;

    // Continuous scanning!!!
    // Are there ever interupts though...
    // Interrupt handlers???
    // Could still have some form of interrupt esque construct...
    kb_SetMode(MODE_3_CONTINUOUS);

    do {

        /* Update kb_Data (NON-BLOCKING!) */
        // Potential need for driver!!!!
        // And or interrupt Handlers???
        // kb_Scan();
        // Scanning will have to happen a lot I guess...

        key = kb_Data[7];

        if (key & kb_Down) {
            if (!down) {
                down = 1;

                gfx_FillScreen(155);
                PrintCentered("Pressed!");
            }
        } else if (down) {
            down = 0;
            gfx_FillScreen(255);
        }
    } while (kb_Data[6] != kb_Clear);

    gfx_End();

    return 0;
}

/* Prints a screen centered string */
void PrintCentered(const char *str)
{
    gfx_PrintStringXY(str,
                      (LCD_WIDTH - gfx_GetStringWidth(str)) / 2,
                      (LCD_HEIGHT - 8) / 2);
}