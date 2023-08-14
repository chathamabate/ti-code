#pragma once

#include "cxxutil/core/input.h"
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace gui {

    class Pane : public core::SafeObject {
    private:
        bool inFocus;
    protected:
        Pane(uint8_t memChnl);
        Pane();

        virtual ~Pane();
    public:
        virtual void render(uint24_t x, uint8_t y) = 0;

        // Key scanning should be done before this call.
        virtual void update(core::KeyManager *km) = 0;

        virtual uint24_t getWidth() = 0;
        virtual uint8_t getHeight() = 0;

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


