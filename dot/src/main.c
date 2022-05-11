
#include <keypadc.h>
#include <tice.h>
#include <graphx.h>

// LCD_WIDTH
// LCD_HEIGHT

int main(void) {
    uint16_t x = 0;
    uint8_t y = 0;
    uint8_t move = 0;

    gfx_Begin();

    gfx_SetColor(1);
    gfx_HorizLine(0, y, LCD_WIDTH);

    do {
        kb_Scan();

        if (kb_IsDown(kb_KeyUp)) {
            y--;

            if (y >= LCD_HEIGHT) {
                y = LCD_HEIGHT - 1;
            }

            move = 1;
        }

        if (kb_IsDown(kb_KeyDown)) {
            y++;

            if (y >= LCD_HEIGHT) {
                y = 0;
            }
            
            move = 1;
        }

        if (move) {
            gfx_FillScreen(255);
            gfx_HorizLine(0, y, LCD_WIDTH);
            move = 0;
        }


    } while (!kb_IsDown(kb_KeyClear));

    gfx_End();

    return 0;
}