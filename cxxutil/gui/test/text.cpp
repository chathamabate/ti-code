#include "./text.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/unit/unit.h"
#include "graphx.h"

#include <cstdio>
#include <cxxutil/gui/text.h>

using namespace cxxutil;

class TextBlockTestCase : public unit::TestCase {
private:
    const char * const initialText;
    const uint24_t clipWidth;
    const size_t expectedNumLines;
    const char * const * const  expectedLines;

    gui::TextBlock *tb;

    virtual void attempt(unit::TestContext *tc) override {
        // Width scale is all that matters here.
        // NOTE: Will be width of 8 for all TextBlock Tests.
        gfx_SetTextScale(1, 2);
        gfx_SetMonospaceFont(8); // This is used to make building the tests
                                 // easier.

        this->tb = new gui::TextBlock(1, this->initialText, this->clipWidth);

        // First confirm all lines have a valid width.
        for (size_t i = 0; i < this->tb->getLines()->getLen(); i++) {
            char widLbl[10];
            sprintf(widLbl, "wid %u", i);

            uint24_t lineWidth = gfx_GetStringWidth(
                    this->tb->getLines()->get(i)->getArr());
            
            tc->lblAssertTrue(widLbl, lineWidth <= this->clipWidth);
        }

        // Then check for equality against the expected.
        tc->lblAssertEqUInt("len", this->expectedNumLines, 
                this->tb->getLines()->getLen());

        for (size_t i = 0; i < this->expectedNumLines; i++) {
            char lnLbl[10];
            sprintf(lnLbl, "%u", i);
            tc->lblAssertEqStr(lnLbl, 
                    this->expectedLines[i], 
                    this->tb->getLines()->get(i)->getArr());
        }
    }

    virtual void finally() override {
        delete this->tb;
    }

public:    
    TextBlockTestCase(const char *n, const char *txt, uint24_t clipW,
            size_t expNumLines, const char * const *expLines) 
        : unit::TestCase(n), 
        initialText(txt), 
        clipWidth(clipW),
        expectedNumLines(expNumLines),
        expectedLines(expLines) {
    }
};

static TextBlockTestCase SIMPLE_TB1(
        "Simple TB 1", 
        "AAA", 
        80, 
        1,
        (const char *[]){
            "AAA"
        }
);

static TextBlockTestCase SIMPLE_TB2(
        "Simple TB 2", 
        "    AAA         ", 
        80, 
        1,
        (const char *[]){
            "AAA"
        }
);

static TextBlockTestCase SIMPLE_TB3(
        "Simple TB 3", 
        "AAA  A", 
        80, 
        1,
        (const char *[]){
            "AAA A"
        }
);

static TextBlockTestCase SIMPLE_TB4(
        "Simple TB 4", 
        "AAA  AAAA  AAA", 
        80, 
        2,
        (const char *[]){
            "AAA AAAA",
            "AAA"
        }
);

static TextBlockTestCase SIMPLE_TB5(
        "Simple TB 5", 
        "A AAAA AAA", 
        40, 
        3,
        (const char *[]){
            "A",
            "AAAA",
            "AAA"
        }
);

static TextBlockTestCase SIMPLE_TB6(
        "Simple TB 6", 
        "A  AAAAAA    AAA", 
        40, 
        3,
        (const char *[]){
            "A AAA",
            "AAA",
            "AAA"
        }
);

static TextBlockTestCase SIMPLE_TB7(
        "Simple TB 7", 
        "AAAAAAAAAAA AAAA", 
        40, 
        4,
        (const char *[]){
            "AAAAA",
            "AAAAA",
            "A",
            "AAAA"
        }
);

static TextBlockTestCase SIMPLE_TB8(
        "Simple TB 8", 
        "AA A A", 
        40, 
        2,
        (const char *[]){
            "AA A",
            "A"
        }
);

static TextBlockTestCase EMPTY_TB(
        "Empty TB", 
        "                 ", 
        40, 
        0,
        (const char *[]){
        }
);

static TextBlockTestCase LONG_TB(
        "Long TB", 
        "AA AAAA   A A A AAAAAAA A A AAA", 
        40, 
        6,
        (const char *[]){
            "AA",
            "AAAA",
            "A A A",
            "AAAAA",
            "AA A",
            "A AAA",
        }
);


const size_t TEXT_SUITE_LEN = 10;
static cxxutil::unit::TestCase * const TEXT_SUITE_TESTS[TEXT_SUITE_LEN] = {
    &SIMPLE_TB1,
    &SIMPLE_TB2,
    &SIMPLE_TB3,
    &SIMPLE_TB4,
    &SIMPLE_TB5,
    &SIMPLE_TB6,
    &SIMPLE_TB7,
    &SIMPLE_TB8,

    &EMPTY_TB,
    &LONG_TB,
};

unit::TestSuite cxxutil::gui::TEXT_SUITE("Text Suite", TEXT_SUITE_TESTS, TEXT_SUITE_LEN);
