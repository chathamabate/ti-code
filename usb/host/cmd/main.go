package main

import (
	"fmt"
	"log"
	"time"

	"github.com/google/gousb"
)

// TI-84 Vendor ID.
const VENDOR_ID = 0x0451
const PRODUCT_ID = 0xE008

func main() {
    err := innerMain()
    if err != nil {
        log.Fatal(err)
    }
}

func innerMain() error {
	// Initialize a new Context.
    // I think, we turn this on, and wait!
	ctx := gousb.NewContext()
	defer ctx.Close()

    var err error
    var dev *gousb.Device

    const MAX_TRIES = 10
    for i := 0; i < MAX_TRIES; i++ {
        log.Printf("Polling for TI-84... (Try %d)\n", i + 1)
        dev, err = ctx.OpenDeviceWithVIDPID(VENDOR_ID, PRODUCT_ID)

        if err != nil {
            return fmt.Errorf("Error polling for TI-84 (%w)", err)
        }

        if dev != nil {
            break
        }

        time.Sleep(time.Second * 1)
    }

    if dev == nil {
        return fmt.Errorf("Could not find TI-84")
    }

    defer dev.Close()

    log.Printf("TI-84 found!")

    // Let's print out the different configurations.
    // I believe right now there is a configuration for the 3 different 
    // transfer types...
    // Don't need this anymore
    //for i, tcfg := range dev.Desc.Configs {
    //    log.Printf("%d: %d", i, len(tcfg.Interfaces))
    //}

    cfgNum, err := dev.ActiveConfigNum()
    if err != nil {
        return fmt.Errorf("Error getting active config number (%w)", err)
    }

    cfg, err := dev.Config(cfgNum)
    if err != nil {
        return fmt.Errorf("Error getting active config (%w)", err)
    }

    intfr, err := cfg.Interface(0, 0)
    if err != nil {
        return fmt.Errorf("Error getting interface (%w)", err)
    }
    defer intfr.Close()

    log.Printf("Aquired interface 0")

    var outEp *gousb.OutEndpoint
    var inEp *gousb.InEndpoint

    for _, epd := range intfr.Setting.Endpoints {
        if epd.Direction == gousb.EndpointDirectionIn {
            inEp, err = intfr.InEndpoint(epd.Number)
        } else {
            outEp, err = intfr.OutEndpoint(epd.Number)
        }

        if err != nil {
            return fmt.Errorf("Error getting endpoint (%w)", err)
        }
    }
    
    if false {
        inEp.Read(nil)
    }

    const BUF_SIZE = 100
    buf := make([]byte, BUF_SIZE)
    copy(buf, "Hello")

    const WRITES = 1
    for i := 0; i < WRITES; i++ {
        log.Printf("Writing buf (Write %d)", i + 1)
        _, err := outEp.Write(buf)  // This seems to block?
        if err != nil {
            return fmt.Errorf("Error writing to device (%w)", err)
        }
        
        time.Sleep(time.Second * 1)
    }

    log.Printf("Exiting...\n")
    return nil
}

