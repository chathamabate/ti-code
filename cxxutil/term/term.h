#pragma once

#include <stdint.h>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace term {
    // Supported font effects:
    // Foreground Color (8 Normal colors + 8 Bright Colors) = 16 total
    // F
    //
    //

    // Ansi color offsets.
    constexpr uint8_t ANSII_BLACK = 0;
    constexpr uint8_t ANSII_RED = 1;
    constexpr uint8_t ANSII_GREEN = 2;
    constexpr uint8_t ANSII_YELLOW = 3;
    constexpr uint8_t ANSII_BLUE = 4;
    constexpr uint8_t ANSII_MAGENTA = 5;
    constexpr uint8_t ANSII_CYAN = 6;
    constexpr uint8_t ANSII_WHITE = 7;
    
    struct cell_style_t {
        uint24_t fgColor : 3; 
        uint24_t bgColor : 3;
        uint24_t strong : 1;  
        uint24_t underline : 1;
        uint24_t slowBlink : 1;
        uint24_t fastBlink : 1;
        uint24_t strikeThru : 1;
    };

    struct cell_state_t {
        cell_style_t style;
        char character;
    };
    
    // The point of this will ultimately be to allow for control character
    // sequences to actually be interpreted by the terminal...
    // This will be pretty bare bones tbh, but still cool!
    class Terminal : public core::SafeObject {

    public:
        // Can we implement scrolling, is this a good or bad feature???
        // I think scrolling is good!

        // Maybe store an entire backlog of text???
        // hmmmm... scrolling could be cool...
        
    };
}}
