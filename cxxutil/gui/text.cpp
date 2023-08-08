#include "./text.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"

#include <cctype>
#include <graphx.h>
#include <string.h>

using namespace cxxutil::gui;

// This copies words from message into the given string builder.
// Undefined behavoir if msg is null.
//
// Returns nullptr if there is no next line.
static cxxutil::core::SafeArray<char> *nextWordLine(uint8_t memChnl, 
        cxxutil::core::CoreList<char> *strBuilder, 
        const char *msg, uint24_t clipWidth) {
    // This may be different depending on font configuration.
    const uint24_t SPACE_WIDTH = gfx_GetCharWidth(' ');

    size_t i = 0;      

    bool firstWord = true; // True if we are still processing
                           // the first word.
                           
    size_t lineWidth = 0;

    while (true) {
        while (msg[i] != '\0' && isspace(msg[i])) {
            i++;
        }

        if (msg[i] == '\0') {
            break;
        }

        // We have made it to a non-space character!

        // The exlusive end of the word, and a lower bound on the
        // width of the word.
        size_t wordBound = i;   
        uint24_t wordWidth = 0;

        // midBound is either the exlusive end of the word, or
        // the exclusive end of the line, whichever comes first.
        //
        // midWidth is the width of the string from i to midBound.
        size_t midBound = i;   
        uint24_t midWidth  = 0;

        // width of next character to be processed.
        size_t nextCharWidth = gfx_GetCharWidth(msg[i]);

        // We process until:
        // 1) We hit the end of the string.
        // 2) We hit a space.
        // or 3) The final width after adding the word exceeds the clipWidth.
        while (msg[wordBound] != '\0' && !isspace(msg[wordBound]) && 
                lineWidth + (firstWord ? 0 : SPACE_WIDTH) + 
                wordWidth + nextCharWidth <= clipWidth) {
            midWidth += nextCharWidth;
            midBound++;

            wordWidth += nextCharWidth;
            wordBound++; 

            nextCharWidth = gfx_GetCharWidth(msg[wordBound]);
        }

        // Now we process until:
        // 1) we hit the end of the string.
        // 2) we hit a space.
        // or 3) the width of the word alone is too large for one line.
        while (msg[wordBound] != '\0' && !isspace(msg[wordBound]) && 
                wordWidth <= clipWidth) {
            wordWidth += gfx_GetCharWidth(msg[wordBound]);
            wordBound++; 
        }

        // Case where our word can fit on a single line, but not this one.
        // Simply exit. (NOTE: it is impossible here for firstWord = true)
        if (wordWidth <= clipWidth && lineWidth + wordWidth > clipWidth) {
            break;
        }

        // At this point midBound marks the exact exlusive index we can add
        // to until a space is hit, the line is full, or both.
        
        if (!firstWord) {
            strBuilder->add(' ');
            lineWidth += SPACE_WIDTH;
        } else {
            firstWord = false;
        }

        // Add our characters.
        for (; i < midBound; i++) {
            strBuilder->add(msg[i]);
        } 

        lineWidth += midWidth;

        // Here, if our next character exists and is non-space,
        // we must've stopped processing because our word was too long
        // for a single line. just exit!
        if (!isspace(msg[i]) || msg[i] == '\0') {
            break;     
        }
    }   

    // Here the "first word" was never added.
    // Nothing to return.
    if (firstWord) {
        return nullptr;
    }

    cxxutil::core::SafeArray<char> *str = strBuilder->toArray();
    strBuilder->clear();

    return str;
}


// A word is a consecutive sequence of non-space characters.
// A break is a consequtivve sequence of space characters.
//
// A line is a sequnce of words separated by single spaces.
// The display width of a single line cannot exceed clipW.
//
// The given message will be transformed into an array of lines.
// 
// A word in the original message is only ever divided if its 
// width alone exceeds the clipW, in this case, its contents will
// be displayed over multiple lines.
TextBlock::TextBlock(uint8_t memChnl, const char *msg, uint24_t clipW)
    : core::SafeObject(memChnl) {
    
    // Msg must be non-null.
    if (!msg) {
        this->lines = 
            new core::SafeArray<core::SafeArray<char> *>(memChnl, 0);

        return;
    }

    core::CoreList<core::SafeArray<char> *> *blkBuilder = 
        new core::CoreList<core::SafeArray<char> *>(memChnl);

    core::CoreList<char> *strBuilder = 
        new core::CoreList<char>(memChnl);

    // What if the string is just spaces though???
    // These are things we need to think about...
    // There might not be a "clean" way to do this..

    size_t i = 0;        
    while (msg[i] != '\0') {
        // Ignore initial leading spaces.
        while (msg[i] != '\0' && isspace(msg[i])) {
            i++;
        }

        if (msg[i] == '\0') {
            break;
        }

        size_t width = 0;

        // Add words one at a time until the width is exceeded,
        // C'mon mate, you can do this... I know it...
        // than stop?
        // Word than space then word then space....

        while (width < clipW) {

        }
         
        // Chop off a line using the string builder.
        // Add it to the blkbuilder.
        // continue.
    }

    delete strBuilder;

    this->lines = blkBuilder->toArray();
    delete blkBuilder;
}

TextBlock::TextBlock(const char *msg, uint24_t clipW) 
    : TextBlock(core::CXX_DEF_CHNL, msg, clipW) {
}

TextBlock::~TextBlock() {
}


Terminal::Terminal(uint8_t memChnl, const char *title) 
    : core::SafeObject(memChnl) {

}

Terminal::Terminal(const char *title) : Terminal(core::CXX_DEF_CHNL, title) {
}

Terminal::~Terminal() {
    // Fill this bad boy in.
}


