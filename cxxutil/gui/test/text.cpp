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
        this->tb = new gui::TextBlock(1, this->initialText, this->clipWidth);
        
        tc->lblAssertEqUInt("len", this->expectedNumLines, 
                this->tb->getLines()->getLen());

        for (size_t i = 0; i < this->expectedNumLines; i++) {
            char lnLbl[10];
            sprintf(lnLbl, "%u", i);
            tc->lblAssertEqStr(lnLbl, 
                    this->expectedLines[i], 
                    tb->getLines()->get(i)->getArr());
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

const size_t TEXT_SUITE_LEN = 1;
static cxxutil::unit::TestCase * const TEXT_SUITE_TESTS[TEXT_SUITE_LEN] = {
    &SIMPLE_TB1
};

unit::TestSuite cxxutil::gui::TEXT_SUITE("Text Suite", TEXT_SUITE_TESTS, TEXT_SUITE_LEN);
