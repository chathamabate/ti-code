#include "./text_block.h"

#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/unit/unit.h"
#include "graphx.h"
#include "ti/screen.h"

#include <cstdio>
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

using namespace cxxutil;

static const gui::text_info_t TC_TEXT_INFO = {
    .widthScale = 1,
    .heightScale = 2,
    .monospace = 8,
};

class TextBlockTestCase : public unit::TestCase {
private:
    const char * const initialText;
    const uint24_t clipWidth;
    const size_t expectedNumLines;
    const char * const * const  expectedLines;

    gui::TextBlock *tb;

    virtual void attempt(unit::TestContext *tc) override {
        this->tb = new gui::TextBlock(1, &TC_TEXT_INFO, this->initialText, this->clipWidth);
        const core::SafeArray<const core::SafeArray<char> *> *lines = tb->getLines();

        size_t n = lines->getLen() < this->expectedNumLines 
            ? lines->getLen() : this->expectedNumLines;

        for (size_t i = 0; i < n; i++) {
            char lnLbl[10];
            sprintf(lnLbl, "line %u", i);

            tc->lblAssertEqStr(lnLbl, 
                    this->expectedLines[i], 
                    lines->get(i)->getArr());
        }

        if (n < lines->getLen()) {
            tc->fatal("Too many lines.");
        }

        if (n < this->expectedNumLines) {
            tc->fatal("Too few lines.");
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

static TextBlockTestCase SIMPLE_TB9(
        "Simple TB 9", 
        "A A A AAAAAA", 
        40, 
        3,
        (const char *[]){
            "A A A",
            "AAAAA",
            "A",
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

static TextBlockTestCase LONG_TB1(
        "Long TB 1", 
        "AAA A    AAAAAAAAA A A A AAAA A A", 
        40, 
        6,
        (const char *[]){
            "AAA A",
            "AAAAA",
            "AAAA",
            "A A A",
            "AAAA",
            "A A",
        }
);

static TextBlockTestCase LONG_TB2(
        "Long TB 2", 
        "AA AA AA AA A A A AAAAAA AA", 
        40, 
        5,
        (const char *[]){
            "AA AA",
            "AA AA",
            "A A A",
            "AAAAA",
            "A AA", 
        }
);

const size_t TEXT_BLOCK_SUITE_LEN = 12;
static cxxutil::unit::TestTree * const 
TEXT_BLOCK_SUITE_TESTS[TEXT_BLOCK_SUITE_LEN] = {
    &SIMPLE_TB1,
    &SIMPLE_TB2,
    &SIMPLE_TB3,
    &SIMPLE_TB4,
    &SIMPLE_TB5,
    &SIMPLE_TB6,
    &SIMPLE_TB7,
    &SIMPLE_TB8,
    &SIMPLE_TB9,

    &EMPTY_TB,

    &LONG_TB1,
    &LONG_TB2,
};

unit::TestSuite cxxutil::gui::TEXT_BLOCK_SUITE(
        "Text Block Suite", TEXT_BLOCK_SUITE_TESTS, TEXT_BLOCK_SUITE_LEN);
