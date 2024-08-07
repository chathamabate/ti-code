#include "./text_block.h"

#include "cxxutil/core/mem.h"
#include "ti/screen.h"

#include <cctype>
#include <cstdio>
#include <graphx.h>
#include <string.h>

using namespace cxxutil::gui;

// I think it may be better just to have a separate class???
// For this type of stuff?
// Might be slightly better...

// Undefined behavoir if msg is null.
//
// Returns the index of the next character which needs to be processed.
static size_t buildLine(cxxutil::core::CoreList<char> *strBuilder, 
        const text_info_t *ti, const char *msg, uint24_t clipWidth) {
    // Height scale not necessary here, but still used.
    gfx_SetTextScale(ti->widthScale, ti->heightScale);
    gfx_SetMonospaceFont(ti->monospace);

    // This may be different depending on font configuration.
    const uint24_t SPACE_WIDTH = gfx_GetCharWidth(' ');

    // Index of the next character to be processed.
    size_t i = 0;      

    bool firstWord = true; // True if we are still processing
                           // the first word.
                           
    // Combined width in pixels of all characters added to the
    // give strBuilder.
    size_t lineWidth = 0;

    while (true) {
        while (isspace(msg[i])) {
            i++;
        }

        if (msg[i] == '\0') {
            break;
        }

        // We have made it to a non-space character!

        // msg[i:wordBound] will be the word which is currently 
        // being processed. wordBound will stop increasing once
        // 1) a space or \0 is hit.
        // or 2) width(msg[i:wordBound]) exceeds clipWidth.
        size_t wordBound = i;   
        uint24_t wordWidth = 0; // = width(msg[i:wordBound]).

        // msg[i:midBound] will be the character which we may add to
        // our line. midBound stops increasing once
        // 1) a space or \0 is hit.
        // or 2) wid(msg[i:midBound]) is its maximum possible value
        // before adding msg[i:midBound] to line exceed clipWidth.
        size_t midBound = i;   
        uint24_t midWidth  = 0;

        // width of next character to be processed.
        size_t nextCharWidth = gfx_GetCharWidth(msg[i]);

        // We go until:
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

        // msg[i:midBound] will be the characters we add (if we choose to)
        // Thus, if midBound == i, there are no characters we can add
        // without exceeding clipWidth! Just exit.
        if (midBound == i) {
            break;
        }
        
        // After this point, we know there are character which can be added,
        // now we must decided if we should add them or not.

        // Now we go until:
        // 1) we hit the end of the string.
        // 2) we hit a space.
        // or 3) the width of the word alone is too large for one line.
        while (msg[wordBound] != '\0' && !isspace(msg[wordBound]) && 
                wordWidth <= clipWidth) {
            wordWidth += gfx_GetCharWidth(msg[wordBound]);
            wordBound++; 
        }

        // Case where our word can fit on a single line, but not this one.
        // Simply exit. 
        if (wordWidth <= clipWidth && 
                lineWidth + (firstWord ? 0 : SPACE_WIDTH) + wordWidth > clipWidth) {
            break;
        }

        // At this point midBound marks the exact exlusive index we can add
        // to until a space is hit or the line is full.
        
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

        // If our next char to process is not a space character,
        // 1) We only added a piece of a word, 
        //    impling we have reached clipwidth.
        // 2) We have reached the end of the string.
        //
        // In both cases we can exit.
        if (!isspace(msg[i])) {
            break;     
        }
    }   

    return i;
}

TextBlock::TextBlock(uint8_t memChnl, const text_info_t *ti, const char *msg, uint24_t clipWidth) 
    : core::SafeObject(memChnl), textInfo(ti) {
    if (!msg) {
        this->lines = 
            new core::SafeArray<const core::SafeArray<char> *>(memChnl, 0);

        return;
    }

    core::CoreList<const core::SafeArray<char> *> *blkBuilder = 
        new core::CoreList<const core::SafeArray<char> *>(memChnl);

    core::CoreList<char> *strBuilder = 
        new core::CoreList<char>(memChnl);

    const char *m = msg;
    size_t processed = buildLine(strBuilder, this->textInfo, m, clipWidth);

    while (strBuilder->getLen() > 0) {
        strBuilder->add('\0');

        blkBuilder->add(strBuilder->toArray());
        strBuilder->clear();

        m = &(m[processed]);

        processed = buildLine(strBuilder, this->textInfo, m, clipWidth);
    }

    delete strBuilder;

    this->lines = blkBuilder->toArray();
    delete blkBuilder; 
}

TextBlock::TextBlock(const text_info_t *ti, const char *msg, uint24_t clipWidth) 
    : TextBlock(core::CXX_DEF_CHNL, ti, msg, clipWidth) {
}

TextBlock::~TextBlock() {
    for (size_t i = 0; i < this->lines->getLen(); i++) {
        delete this->lines->get(i); 
    }

    delete this->lines;
}
