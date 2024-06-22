package main


import (
	"fmt"
	"log"
	"github.com/google/gousb"
)

// TI-84 Vendor ID.
const VENDOR_ID = 0x0451
const PRODUCT_ID = 0xE008

func main() {
	// Initialize a new Context.
	ctx := gousb.NewContext()
	defer ctx.Close()

    // Maybe first let's just read all devices??
    dev, err := ctx.OpenDeviceWithVIDPID(VENDOR_ID, PRODUCT_ID)
	if err != nil || dev == nil {
		log.Fatalf("Could not open a device: %v", err)
	}
	defer dev.Close()

    fmt.Println("Device Successfully Found") 
    productName, err := dev.Product()
    if err != nil {
        log.Fatalf("Could not get product name")
    }
    fmt.Printf("Product Name: %s\n", productName)
}

