#include <cxxutil/core/input.h>
#include <ti/screen.h>
#include <graphx.h>
#include <sys/lcd.h>


using namespace cxxutil;

int main(void) {
    // Ok, nice, this works!
    os_ClrHome(); 

    uint16_t *vram = (uint16_t *)lcd_Ram;

    for (int r = 0; r < 240; r++) {
        uint8_t r8 = (r * 32) / 240;
        uint8_t g8 = 0;
        uint8_t b8 = ((240 - r - 1) * 32) / 240;
           
        // uint16_t color = gfx_RGBTo1555((r * 255) / 240, 0, 0);

        uint16_t color = (r8 << 11) | (g8 << 5) | b8;

        for (int c = 0; c < 320; c++) {
            vram[r*320 + c] = color;
        }
    }

    while (!os_GetCSC());

    os_ClrHome();
}
