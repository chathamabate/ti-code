#include "cxxutil/core/mem.h"
#include "keypadc.h"
#include "sys/timers.h"
#include <ti/screen.h>
#include <ti/getcsc.h>
#include <stdlib.h>
#include <stdio.h>
#include <cxxutil/term/term.h>

#include <usbdrvce.h>
#include <cxxutil/core/input.h>

#include <graphx.h>
#include <fileioc.h>

#define char_buf_size 50
static char char_buf[char_buf_size];

#define println(str) os_PutStrLine(str); os_NewLine()
#define printlnf(...) \
    snprintf(char_buf, char_buf_size, __VA_ARGS__); \
    os_PutStrLine(char_buf); os_NewLine()

#define TRANSFER_BUF_SIZE 100


// Is this the root hub?
// I think this may start as a host, but is changed to a device after
// connecting with my laptop???

static usb_error_t usb_callback(usb_event_t event, void *event_data, 
        usb_callback_data_t *callback_data) {
    (void)event;
    (void)event_data;
    (void)callback_data;

    return USB_SUCCESS;
}

int main(void) {
    os_ClrHome();

    uint8_t handle = ti_Open("MyFont", "r");
    if (!handle) {
        os_PutStrLine("Could not locate font");
        cxxutil::core::waitClear();
        return 1;
    }
    const uint8_t *font = (const uint8_t *)ti_GetDataPtr(handle);

    if (!ti_Close(handle)) {
        os_PutStrLine("Could not close font AppVar");
        cxxutil::core::waitClear();
        return 1;
    }

    cxxutil::term::terminal_config_t config = {
        .fontData = font,
        .colorMap = cxxutil::term::DEF_COLOR_MAP,

        .viewX = 0,
        .viewY = 0,

        .widthScale = 1,
        .heightScale = 2,

        .rows = 10,
        .cols = 10,
        .pad = 1
    };

    cxxutil::term::Terminal *T = 
        new cxxutil::term::Terminal(5, config);

    gfx_Begin();
    gfx_FillScreen(255);

    while (!kb_IsDown(kb_KeyClear)) {
        delay(200);
        T->render();
        kb_Scan();
    }

    gfx_End();

    os_ClrHome();

    delete T;
    cxxutil::core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;


    // Ok, time to figure out how to use this usb stuff??
    //usb_error_t usb_Init(usb_event_callback_t handler, 
    //usb_callback_data_t *data,
    //const usb_standard_descriptors_t *device_descriptors,
    //usb_init_flags_t flags);

    /*usb_error_t usb_err;

    println("Initing USB");

    usb_err = usb_Init(usb_callback, NULL, NULL, USB_DEFAULT_INIT_FLAGS);
    if (usb_err != USB_SUCCESS) {
        println("Error Setting up USB");
        goto ending;
    }

    // Need to find our root I believe??
    // Then read from the buffer?

    println("Polling for events...");
    // Or maybe read??

    while (1) {
        km->scanFocusedKeys();
        if (km->isKeyDown(cxxutil::core::CXX_KEY_Clear)) {
            break;
        }

        usb_err = usb_HandleEvents();

        if (usb_err != USB_SUCCESS) {
            println("Error handling events...");
            break;
        }

        // Tenth of a second.
        msleep(100);
    }

ending:
    usb_Cleanup();
    println("Exiting");
    */
}
