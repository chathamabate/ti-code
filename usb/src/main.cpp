#include <ti/screen.h>
#include <ti/getcsc.h>
#include <stdlib.h>
#include <stdio.h>

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
    while ((device = usb_FindDevice(NULL, device, USB_SKIP_NONE))) {
        // This successfully finds my laptop!
        // Can we do more tho??
        println("Found Device"); 

        size_t config_desc_size = usb_GetConfigurationDescriptorTotalLength(device, 0);
        void *config_desc = malloc(config_desc_size);

        size_t trans;
        usb_err = usb_GetDescriptor(device, USB_CONFIGURATION_DESCRIPTOR, 0, 
                config_desc, config_desc_size, &trans);

        free(config_desc);

        if (usb_err != USB_SUCCESS) {
            println("Error getting descriptor");
            goto ending;
        } else {
            char buf[20];
            snprintf(buf, 20, "SZ: %zu", trans);
            println(buf);
        }
    }


    println("Successfully set up USB");
    while (!os_GetCSC());

ending:

    println("Cleaning Up");
    usb_Cleanup();

    while (!os_GetCSC());
    
    os_ClrHome();
}
