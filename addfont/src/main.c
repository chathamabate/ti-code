
#include "addfont/src/font.h"
#include <ti/screen.h>
#include <keypadc.h>
#include <tice.h>
#include <fileioc.h>

#define println(str) \
    os_PutStrLine(str); \
    os_NewLine()

void saveFont() {
    println("Openning AppVar: MyFont");

    uint8_t handle = ti_Open("MyFont", "w");
    if (!handle) {
        println("Error openning AppVar");
        return;
    }

    println("Writing font");
    size_t written = ti_Write(font, sizeof(uint8_t), FONT_LEN, handle);

    if (written != FONT_LEN) {
        println("Error writing font");
        return;
    }

    if (!ti_IsArchived(handle)) {
        println("Archiving font");
        if (!ti_ArchiveHasRoom(handle)) {
            println("Not enough room in archive");
            return;
        }

        if (!ti_SetArchiveStatus(1, handle)) {
            println("Error archiving font");
            return;
        }
    }

    println("Font is archived");

    println("Closing AppVar");

    if (!ti_Close(handle)) {
        println("Error closing AppVar");
        return;
    }
}

int main() {
    os_ClrHome();

    saveFont();

    println("Press Clr to Exit");
    do {
        delay(100);
        kb_Scan();
    } while (!kb_IsDown(kb_KeyClear));

    return 0;
}
