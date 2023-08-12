#pragma once

#include "cxxutil/core/data.h"
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
    
    // NOTE: It is assumed that this describes text of the default 
    // font. (i.e. 8x8 defualt size, not monospace)
    typedef struct {
        uint8_t widthScale;     // Actual width = 8 * widthScale.
        uint8_t heightScale;    // Actual height = 8 * heightScale.

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

        inline const text_info_t *getTextInfo() const {
            return this->textInfo;
        }
    };

    typedef struct {
        // x and y can be given at render time.
        
        uint8_t height;

        uint24_t lineWidth;
        uint8_t scrollBarWidth;
        // Total Width = lineWidth + scrollBarWidth.

        // BG color of the pane.
        uint8_t bgColor;
        uint8_t scrollBarColor;

        // Vertical space which separates lines.
        uint8_t vertLineSpace;
    } scroll_text_pane_info_t;

    typedef struct {
        size_t blockInd;
        size_t lineInd;
    } tp_index_t;  // Text pane index.

    // Will probs need ot use clipping!
    // Text Printing can be negative!
    // void gfx_SetClipRegion(int xmin, int ymin, int xmax, int ymax);
    
    // A scrolling text pane provides a fixed sized display for viewing
    // a variable set of TextBlocks. While the pane exists, TextBlocks can
    // be added and scrolled through accordingly.
    //
    // The width/height of a single line from a text block will be scaled with a base
    // factor of 8. The height of a single line can never exceed the height of 
    // the pane itself.
    class ScrollTextPane : public core::SafeObject {
    private:
        const scroll_text_pane_info_t * const paneInfo; 

        core::CoreList<const TextBlock *> *blocks;

        // As no line has a height larger than the height of the pane,
        // it is possible for any line to be displayed in its entirety.


        // (blockInd, lineInd) refers to the "focused line".
        // All visible lines will be rendered relative to this line.
        tp_index_t focusInd;

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
    public:
        ScrollTextPane(uint8_t memChnl, const scroll_text_pane_info_t *stpi);
        ScrollTextPane(const scroll_text_pane_info_t *stpi);
        ~ScrollTextPane();

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
        // NOTE: For now, logging will alwas call gotoBottom()
        bool log(const text_info_t *ti, const char *msg);

        // These getters below will help with testing.

        inline bool getTop() const {
            return this->top;
        }

        inline tp_index_t getFocusInd() const {
            return this->focusInd;
        }

        inline uint8_t getLineHeight(tp_index_t i) const {
            return this->blocks->get(i.blockInd)->getTextInfo()->heightScale * 8;
        }
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


