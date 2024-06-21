#include <ti/screen.h>
#include <ti/getcsc.h>

#include <usbdrvce.h>

#define println(str) os_PutStrLine(str); os_NewLine()

static usb_error_t usb_callback(usb_event_t event, void *event_data, 
        usb_callback_data_t *callback_data) {
    (void)event;
    (void)event_data;
    (void)callback_data;

    return USB_SUCCESS;
}

int main(void) {
    os_ClrHome();

    // Ok, time to figure out how to use this usb stuff??
    //usb_error_t usb_Init(usb_event_callback_t handler, 
    //usb_callback_data_t *data,
    //const usb_standard_descriptors_t *device_descriptors,
    //usb_init_flags_t flags);

    usb_error_t usb_err;
    usb_device_t device;

    println("Initing USB");

    usb_err = usb_Init(usb_callback, NULL, NULL, USB_DEFAULT_INIT_FLAGS);

    if (usb_err != USB_SUCCESS) {
        println("Error Setting up USB");
        goto ending;
    }

    device = NULL;
    while ((device = usb_FindDevice(NULL, device, USB_SKIP_HUBS))) {
        println("Found Device"); 
    }


    println("Successfully set up USB");
    while (!os_GetCSC());

ending:

    println("Cleaning Up");
    usb_Cleanup();
    
    os_ClrHome();
}
