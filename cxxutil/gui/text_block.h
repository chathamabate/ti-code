
#pragma once

#include "cxxutil/core/data.h"
#include "graphx.h"
#include <cstdint>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace gui {
    
    // NOTE: It is assumed that this describes text of the default 
    // font. (i.e. 8x8 defualt size, not monospace)
    struct text_info_t {
        // These params are all fed direclty into gfx.
        
        uint8_t widthScale;     // Actual width = 8 * widthScale.
        uint8_t heightScale;    // Actual height = 8 * heightScale.
        uint8_t monospace;

        uint8_t fgColor;
        uint8_t bgColor;

        // Consider making this non inline?
        inline void install() const {
            gfx_SetTextScale(this->widthScale, this->heightScale);
            gfx_SetTextFGColor(this->fgColor);
            gfx_SetTextBGColor(this->bgColor);
            gfx_SetMonospaceFont(this->monospace);
        }

        inline uint8_t getHeight() const {
            return this->heightScale * 8;
        }
    };    

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

        inline const text_info_t *getTextInfo() const {
            return this->textInfo;
        }

        inline uint8_t getLineHeight() const {
            return this->textInfo->getHeight();
        }
    };

}}
