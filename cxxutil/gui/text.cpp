#include "./text.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "ti/screen.h"

#include <cctype>
#include <cstdio>
#include <graphx.h>
#include <string.h>

using namespace cxxutil::gui;

// This copies words from message into the given string builder.
// Undefined behavoir if msg is null.
//
// Returns the index of the next character which needs to be processed.
static size_t buildLine(cxxutil::core::CoreList<char> *strBuilder, 
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

        /*
        char sbuf[30];

        sprintf(sbuf, "Word W: %u", wordWidth);
        os_PutStrFull(sbuf); os_NewLine();

        sprintf(sbuf, "Line W: %u", lineWidth);
        os_PutStrFull(sbuf); os_NewLine();

        uint24_t addWidth = lineWidth + (firstWord ? 0 : SPACE_WIDTH) + wordWidth; 
        sprintf(sbuf, "Add W: %u", addWidth);
        os_PutStrFull(sbuf); os_NewLine();
        */

        // Case where our word can fit on a single line, but not this one.
        // Simply exit. 
        if (wordWidth <= clipWidth && 
                lineWidth + (firstWord ? 0 : SPACE_WIDTH) + wordWidth > clipWidth) {
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
        if (!isspace(msg[i]) && msg[i] != '\0') {
            break;     
        }
    }   

    return i;
}

TextBlock::TextBlock(uint8_t memChnl, const char *msg, uint24_t clipWidth) 
    : core::SafeObject(memChnl) {
    // Msg must be non-null.
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
    size_t processed = buildLine(strBuilder, m, clipWidth);

    while (strBuilder->getLen() > 0) {
        strBuilder->add('\0');

        core::SafeArray<char> *arr = strBuilder->toArray();

        os_PutStrFull(arr->getArr());
        os_NewLine();

        blkBuilder->add(arr);
        strBuilder->clear();

        m = &(m[processed]);

        processed = buildLine(strBuilder, m, clipWidth);
    }

    delete strBuilder;

    this->lines = blkBuilder->toArray();
    delete blkBuilder; 
}

TextBlock::TextBlock(const char *msg, uint24_t clipWidth) 
    : TextBlock(core::CXX_DEF_CHNL, msg, clipWidth) {
}

TextBlock::~TextBlock() {
    for (size_t i = 0; i < this->lines->getLen(); i++) {
        delete this->lines->get(i); 
    }

    delete this->lines;
}


/*
Terminal::Terminal(uint8_t memChnl, const char *title) 
    : core::SafeObject(memChnl) {

}

Terminal::Terminal(const char *title) : Terminal(core::CXX_DEF_CHNL, title) {
}

Terminal::~Terminal() {
    // Fill this bad boy in.
}
*/

