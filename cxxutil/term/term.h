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

    constexpr uint8_t ANSII_STRONG_BLACK = 8;
    constexpr uint8_t ANSII_STRONG_RED = 9;
    constexpr uint8_t ANSII_STRONG_GREEN = 10;
    constexpr uint8_t ANSII_STRONG_YELLOW = 11;
    constexpr uint8_t ANSII_STRONG_BLUE = 12;
    constexpr uint8_t ANSII_STRONG_MAGENTA = 13;
    constexpr uint8_t ANSII_STRONG_CYAN = 14;
    constexpr uint8_t ANSII_STRONG_WHITE = 15;

    constexpr uint8_t TRANSPARENT_IND = 16;

    extern const uint8_t DEF_COLOR_MAP[17];

    // These must be powers of 2.
    constexpr uint8_t FAST_BLINK_FREQ = 2;
    constexpr uint8_t SLOW_BLINK_FREQ = 4;

    struct cell_style_t {
        core::U24 fgColor : 4; 
        core::U24 bgColor : 4;
        core::U24 underline : 1;
        core::U24 slowBlink : 1;
        core::U24 fastBlink : 1;
        core::U24 strikeThru : 1;
    };

    inline bool operator==(const cell_style_t &lhs, const cell_style_t &rhs) {
        return lhs.fgColor == rhs.fgColor &&
            lhs.bgColor == rhs.bgColor &&
            lhs.underline == rhs.underline &&
            lhs.slowBlink == rhs.slowBlink &&
            lhs.fastBlink == rhs.fastBlink &&
            lhs.strikeThru == rhs.strikeThru;
    }

    inline bool operator!=(const cell_style_t &lhs, const cell_style_t &rhs) {
        return !(lhs == rhs);
    }

    struct cell_state_t {
        cell_style_t style;
        char character;
    };

    inline bool operator==(const cell_state_t &lhs, const cell_state_t &rhs) {
        return lhs.character == rhs.character && lhs.style == rhs.style;
    }

    inline bool operator!=(const cell_state_t &lhs, const cell_state_t &rhs) {
        return !(lhs == rhs);
    }

    extern const cell_state_t DEF_CELL_STATE;

    // This assumes we are using an 8x8 bitmap font which can be scaled.
    struct terminal_config_t {
        const uint8_t *fontData;
        const uint8_t *colorMap;

        // Where we will render the terminal on the screen.
        core::U24 viewX;
        uint8_t viewY;

        // GFX Scalings. (1-5)
        uint8_t widthScale;
        uint8_t heightScale;

        uint8_t rows;
        uint8_t cols;

        // This is a pixel value which will appended to the top
        // and bottom of each line. 
        // 2 lines of text will have a 2 * pad spacing.
        uint8_t pad;
    };

    typedef struct {
        // Whether or not the below state actually was rendered.
        uint8_t visible;

        core::SafeArray<cell_state_t> *state;
    } terminal_render_state_t;

    class Terminal : public core::SafeObject {
    private:
        terminal_config_t config;

        core::SafeArray<cell_state_t> *currState;

        // The number of times render has been called.
        // Used for blinking and buffered rendering.
        uint8_t frameNum;

        // Whether or not we should use buffered rendering.
        const uint8_t buffered;
        
        // Only use 1 of these entries when not in buffered mode.
        terminal_render_state_t renders[2];
    public:
        Terminal(uint8_t bufrd, const terminal_config_t &cfg);
        Terminal(uint8_t chnl, uint8_t bufrd, const terminal_config_t &cfg);
        ~Terminal();

        // Assumes swap is called at somepoint after this call.
        void render();

        inline void putCellState(size_t i, const cell_state_t &ct) {
            this->currState->set(i, ct);
        }

        inline void putCellStateRC(size_t r, size_t c, const cell_state_t &ct) {
            this->currState->set((r * this->config.cols) + c, ct);
        }
    };
}}
