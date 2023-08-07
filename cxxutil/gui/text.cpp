#include "./text.h"
#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"

#include <cctype>
#include <graphx.h>
#include <string.h>

using namespace cxxutil::gui;



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


