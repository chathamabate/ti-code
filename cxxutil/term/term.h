#pragma once

#include <stdint.h>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace term {

    // Ansi color offsets.
    constexpr uint8_t ANSII_BLACK = 0;
    constexpr uint8_t ANSII_RED = 1;
    constexpr uint8_t ANSII_GREEN = 2;
    constexpr uint8_t ANSII_YELLOW = 3;
    constexpr uint8_t ANSII_BLUE = 4;
    constexpr uint8_t ANSII_MAGENTA = 5;
    constexpr uint8_t ANSII_CYAN = 6;
    constexpr uint8_t ANSII_WHITE = 7;

    // Each value here should be a pallete index.
    // normal corresponds to regular colors.
    // storng is like "bright" or "bold".
    //
    // NOTE: for now there will be no actual thickness change
    // in bold character, we will just use the stronger color.
    struct terminal_color_map_t {
        uint8_t normal[8];
        uint8_t strong[8];
    };
    
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

    // This assumes we are using an 8x8 bitmap font which can be scaled.
    struct terminal_config_t {
        void *fontData;
        terminal_color_map_t colorMap;

        // This will be the total rows of the terminal,
        // not just those which are visible!
        //
        // (A terminal will contain its own history)
        // Potentially make this unchanging?
        size_t rows;

        // Where we will render the terminal on the screen.
        uint24_t viewX;
        uint8_t viewY;

        uint8_t viewRows;
        uint8_t viewCols;

        // GFX Scalings. (1-5)
        uint8_t widthScale;
        uint8_t heightScale;

        // This is a pixel value which will appended to the top
        // and bottom of each line. 
        // 2 lines of text will have a 2 * pad spacing.
        uint8_t pad;
    };
    
    // The point of this will ultimately be to allow for control character
    // sequences to actually be interpreted by the terminal...
    // This will be pretty bare bones tbh, but still cool!
    class Terminal : public core::SafeObject {
    private:
        terminal_config_t config;

        // 1 dimmensional array of with dimmension rows * viewCols (from config)
        core::SafeArray<cell_state_t> *cells; 

        // First row we are rendering at the top of the screen.
        size_t currRow;

        // We are rendering an offset from currRow.
        void renderCell(uint24_t x, uint8_t y, const cell_state_t &cell);

    public:
        // Can we implement scrolling, is this a good or bad feature???
        // I think scrolling is good!

        // Maybe store an entire backlog of text???
        // hmmmm... scrolling could be cool...

        // This function will render the entire terminal
        void fullRender();
        
        // Note, this will not have a render function perse
    };
}}
