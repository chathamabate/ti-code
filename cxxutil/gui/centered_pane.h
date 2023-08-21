#pragma once

#include "cxxutil/core/mem.h"
#include <cxxutil/gui/pane.h>

namespace cxxutil { namespace gui {

    struct centered_pane_info_t {
        uint8_t titleWidthScale;
        uint8_t titleHeightScale;

        // FG will be the FG of the text rendered.
        uint8_t titleFGColor;

        // BG will be the BG of the entire title row.
        uint8_t titleBGColor;
        
        uint8_t titleBarPadding;

        // This is the color of the space used to 
        // center the internal pane.
        uint8_t borderBGColor;

        // This is the width and height of the entire pane.
        //
        // NOTE: all rendering will execute under the assumption
        // that the title bar and inner pane fit inside 
        // a width x height rectangle.
        uint24_t width;
        uint8_t height;
    };

    // NOTE: T MUST extend Pane!
    template<typename T>
    class CenteredPane : public Pane {
    private:
        const centered_pane_info_t * const paneInfo;

        // NOTE: Inner Pane is NOT "part" of this object.
        // That is it is not deleted by this pane's destructor.
        T * const innerPane;

    public:
        CenteredPane(uint8_t memChnl, const centered_pane_info_t *pi, T *ip) 
            : Pane(memChnl), paneInfo(pi), innerPane(ip) {
        }

        CenteredPane(const centered_pane_info_t *pi, T *ip) 
            : CenteredPane(core::CXX_DEF_CHNL, pi, ip) {
        }

        virtual ~CenteredPane() {
        }

        virtual void render(uint24_t , uint8_t y) const override {
            // Render time my friend.
        }

        virtual uint24_t getWidth() const override {
            return this->paneInfo->width;
        }

        virtual uint8_t getHeight() const override {
            return this->paneInfo->height;
        }

        inline T *getInnerPane() {
            return this->innerPane;
        }
    };
}}
