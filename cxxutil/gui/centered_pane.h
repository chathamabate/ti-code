#pragma once

#include "cxxutil/core/mem.h"
#include "graphx.h"
#include <cxxutil/gui/pane.h>

namespace cxxutil { namespace gui {

    struct centered_pane_info_t {
        uint8_t titleWidthScale;
        uint8_t titleHeightScale;

        // FG will be the FG of the text rendered.
        uint8_t titleFGColor;

        // BG will be the BG of the entire title row.
        uint8_t titleBGColor;
        
        uint8_t titlePadding;

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
        const char * const title;

        // NOTE: Inner Pane is NOT "part" of this object.
        // That is it is not deleted by this pane's destructor.
        T * const innerPane;

    public:
        CenteredPane(uint8_t memChnl, const centered_pane_info_t *pi, 
                const char *t, T *ip) 
            : Pane(memChnl), paneInfo(pi), title(t), innerPane(ip) {
        }

        CenteredPane(const centered_pane_info_t *pi, const char *t, T *ip) 
            : CenteredPane(core::CXX_DEF_CHNL, pi, t, ip) {
        }

        virtual ~CenteredPane() {
        }

        virtual void render(uint24_t x, uint8_t y) const override {
            const centered_pane_info_t * const pi = this->paneInfo;
            // Relative start of the inner border.
            const uint8_t borderRelY = 
                (pi->titleHeightScale * 8) + (pi->titlePadding * 2);

            // First let's render title bar.

            gfx_SetColor(pi->titleBGColor);
            gfx_FillRectangle(x, y, 
                    pi->width, borderRelY);

            gfx_SetTextFGColor(pi->titleFGColor);

            // NOTE: We are assuming this remains transparent.
            gfx_SetTextBGColor(255);

            gfx_SetTextScale(pi->titleWidthScale, pi->titleHeightScale);
            renderClippedText(x + pi->titlePadding, y + pi->titlePadding, 
                    this->title, pi->width - pi->titlePadding);

            // Now we center the inner pane in the left over rectangle.
            // NOTE: we are just assuming it fits. So be careful.
            //
            // That is below we assume outerHeight/Width >= innerHeight/Width
            
            const uint24_t outerWidth = pi->width;
            const uint8_t outerHeight = pi->height - borderRelY;
            
            const uint24_t innerWidth = this->innerPane->getWidth();
            const uint8_t innerHeight = this->innerPane->getHeight();

            // NOTE: these relative to the start of the border,
            // NOT the start of the title bar.
            const uint24_t innerRelX = (outerWidth - innerWidth) / 2;
            const uint8_t innerRelY = (outerHeight - innerHeight) / 2;

            gfx_SetColor(pi->borderBGColor); 

            gfx_FillRectangle(x, y + borderRelY, outerWidth, innerRelY);
            gfx_FillRectangle(x, y + borderRelY + innerRelY + innerHeight,
                    outerWidth, outerHeight - innerRelY - innerHeight);

            gfx_FillRectangle(x, y + borderRelY + innerRelY, innerRelX, innerHeight); 
            gfx_FillRectangle(x + innerRelX + innerWidth, y + borderRelY + innerRelY,
                    outerWidth - innerRelX - innerWidth, innerHeight);

            this->innerPane->render(x + innerRelX, y + borderRelY + innerRelY);
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
