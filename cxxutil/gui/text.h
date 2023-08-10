#pragma once

#include <cstdint>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace gui {

// void gfx_SetTextScale(uint8_t width_scale, uint8_t height_scale);
// void gfx_PrintStringXY(const char *string, int x, int y);
// uint8_t gfx_SetTextFGColor(uint8_t color);
// uint8_t gfx_SetTextBGColor(uint8_t color);
// unsigned int gfx_GetStringWidth(const char *string);
// unsigned int gfx_GetCharWidth(const char c);
//
// typedef enum {
//    gfx_text_clip = 1, /**< Text routines will clip against the defined clip window. */
//     gfx_text_noclip    /**< Default, text routines do not clip (much faster). */
// } gfx_text_options_t;
//
// Probs want no clip if we are careful.
// void gfx_SetTextConfig(uint8_t config);
//
// void gfx_SetTextScale(uint8_t width_scale, uint8_t height_scale);
//
//
// uint8_t gfx_SetColor(uint8_t index);
    
    typedef struct {
        uint8_t widthScale;
        uint8_t heightScale;

        uint8_t fgColor;
        uint8_t bgColor;
    } text_info_t;    

    // A text block is a wrapper class around an array of strings.
    // This class take a single string and splits up its words
    // into lines depending on a given "clip width".
    class TextBlock : public core::SafeObject {
    private:
        const text_info_t * const textInfo;
        core::SafeArray<const core::SafeArray<char> *> *lines;
    public:
        TextBlock(uint8_t memChnl, const text_info_t *ti, const char *msg, uint24_t clipWidth);
        TextBlock(const text_info_t *ti, const char *msg, uint24_t clipWidth);
        ~TextBlock();
        
        inline const core::SafeArray<const core::SafeArray<char> *> * 
            getLines() const {
            return this->lines;
        }
    };

    // This class is for scrolling through TextBlocks.
    class ScrollingTextBox : public core::SafeObject {
    private:
    public:
    };
    
    

    // NOTE: for the Terminal class, all given strings
    // will be copied into dynamic memory.
    /*
    class Terminal : core::SafeObject {
    public: 
        Terminal(uint8_t memChnl, const char *title);
        Terminal(const char *title);

        ~Terminal();

        void render();
        void update();

        // Should we log shit?
        // I'm thinking no log levels.

        void output(uint8_t color, const char *msg);
    }; */
}}


