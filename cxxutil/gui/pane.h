#pragma once

#include "cxxutil/core/input.h"
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace gui {

    // No characters will be rendered past y + clipWidth.
    void renderClippedText(uint24_t x, uint8_t y, 
            const char *text, uint24_t clipWidth);

    class Pane : public core::SafeObject {
    private:
        bool inFocus;
    public:
        Pane(uint8_t memChnl);
        Pane();

        virtual ~Pane();

        virtual void render(uint24_t x, uint8_t y) const = 0;

        virtual uint24_t getWidth() const = 0;
        virtual uint8_t getHeight() const = 0;

        inline bool isInFocus() const {
            return this->inFocus;
        }

        inline void setInFocus(bool f) {
            this->inFocus = f;
        }

        inline void focus() {
            this->setInFocus(true);
        }

        inline void unFocus() {
            this->setInFocus(false);
        }
    };
}}


