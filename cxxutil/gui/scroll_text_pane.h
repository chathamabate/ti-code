
#pragma once

#include "cxxutil/core/input.h"
#include <cxxutil/gui/pane.h>
#include <cxxutil/gui/text_block.h>

namespace cxxutil { namespace gui {
    struct scroll_text_pane_info_t {
        // x and y can be given at render time.
        
        uint8_t height;

        uint24_t lineWidth;
        uint8_t scrollBarWidth;
        // Total Width = lineWidth + scrollBarWidth.

        // BG color of the pane.
        uint8_t bgColor;

        // ScrollbarFG is only render if scroll pane is focused.
        uint8_t scrollBarFGColor;
        uint8_t scrollBarBGColor;

        // Vertical space which separates lines.
        uint8_t vertLineSpace;
    };

    struct tp_index_t {
        size_t blockInd;
        size_t lineInd;

        inline bool operator==(const tp_index_t &i) const {
            return this->blockInd == i.blockInd &&
                this->lineInd == i.lineInd;
        }
    };  // Text pane index.
    
    // Will probs need ot use clipping!
    // void gfx_SetClipRegion(int xmin, int ymin, int xmax, int ymax);
    
    // A scrolling text pane provides a fixed sized display for viewing
    // a variable set of TextBlocks. While the pane exists, TextBlocks can
    // be added and scrolled through accordingly.
    //
    // The width/height of a single line from a text block will be scaled with a base
    // factor of 8. The height of a single line can never exceed the height of 
    // the pane itself.
    class ScrollTextPane : public Pane {
    private:
        const scroll_text_pane_info_t * const paneInfo; 

        core::CoreList<const TextBlock *> *blocks;

        // As no line has a height larger than the height of the pane,
        // it is possible for any line to be displayed in its entirety.

        // (blockInd, lineInd) refers to the "view index".
        // All visible lines will be rendered relative to this line.
        tp_index_t viewInd;

        // If top = true, the focused line will be rendered in its entirety
        // at the top of the pane.
        // 
        // If top = false, the focused lien will be rendered in its entirety
        // at the bottom of the pane.
        // 
        // If there are too few lines to fill the pane, top will always be 
        // true.
        bool top;

        // This is the height of the entire pane should all lines be
        // visible. top can only be false when totalHeight > height.
        uint24_t totalHeight;

        void renderDown(uint24_t x, uint8_t y); 
        void renderUp(uint24_t x, uint8_t y); 
    public:
        ScrollTextPane(uint8_t memChnl, const scroll_text_pane_info_t *stpi);
        ScrollTextPane(const scroll_text_pane_info_t *stpi);

        virtual ~ScrollTextPane() override;

        virtual void render(uint24_t x, uint8_t y) override;

        virtual void update(core::KeyManager *km) override;

        inline virtual uint24_t getWidth() override {
            return this->paneInfo->lineWidth + this->paneInfo->scrollBarWidth;
        }

        inline virtual uint8_t getHeight() override {
            return this->paneInfo->height;
        }

        // Given a valid index into the Pane, this stores the index
        // of the line above/below i into d. 
        //
        // If there is no line above/below i,
        // nothing happens, false is returned.
        //
        // NOTE: UB if index is not valid.
        bool nextUp(tp_index_t i, tp_index_t *d) const;
        bool nextDown(tp_index_t i, tp_index_t *d) const;

        void scrollUp();
        void scrollDown();

        void gotoTop();
        void gotoBottom();

        // NOTE: if msg results in a TextBlock with no lines,
        // the TextBlock will be discarded.
        //
        // NOTE: Also, if the message line height is larger than the height
        // of the pane, nothing will be logged.
        //
        // NOTE: For now, logging will alwas call gotoBottom()
        bool log(const text_info_t *ti, const char *msg);

        // These getters below will help with testing.

        inline bool getTop() const {
            return this->top;
        }

        inline tp_index_t getFocusInd() const {
            return this->viewInd;
        }

        inline uint8_t getLineHeight(tp_index_t i) const {
            return this->blocks->get(i.blockInd)->getTextInfo()->heightScale * 8;
        }
    };
}}


