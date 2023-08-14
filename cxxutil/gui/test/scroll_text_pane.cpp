
#include "./scroll_text_pane.h"

#include "cxxutil/core/data.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/unit/unit.h"
#include "graphx.h"
#include "ti/screen.h"

#include <cstdio>
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

using namespace cxxutil;

static const gui::text_info_t TC_TEXT_INFO_1 = {
    .widthScale = 1,
    .heightScale = 1,
    .monospace = 8,

    // Color fields don't matter.
};

static const gui::text_info_t TC_TEXT_INFO_2 = {
    .widthScale = 1,
    .heightScale = 2,
    .monospace = 8,
};

class ScrollTextPaneTestCase : public unit::TestCase {
private: 
    const gui::scroll_text_pane_info_t stpi;
    // This design ensures the text pane will not be allocated
    // until the test is run!
    virtual void attempt(unit::TestContext *tc) override {
        this->stp = new gui::ScrollTextPane(1, &(this->stpi));
        this->attemptBody(tc);
    }

    virtual void finally() override {
        delete this->stp;
    }

    virtual void attemptBody(unit::TestContext *tc) = 0;

protected:
    gui::ScrollTextPane *stp;

    static constexpr size_t STPTC_LBL_BUF_SIZE = 50;

    void lblAssertFocus(unit::TestContext *tc, const char *lbl,
            bool expTop, size_t expBlockInd, size_t expLineInd) {
        char lblBuf[STPTC_LBL_BUF_SIZE]; 

        const char *topLblParts[2] = {
            lbl, ".top" 
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, topLblParts);
        tc->lblAssertTrue(lblBuf, expTop == this->stp->getTop());

        const char *blockIndLblParts[2] = {
            lbl, ".blockInd"
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, blockIndLblParts);
        tc->lblAssertEqUInt(lblBuf, expBlockInd, this->stp->getFocusInd().blockInd);

        const char *lineIndLblParts[2] = {
            lbl, ".lineInd"
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, lineIndLblParts);
        tc->lblAssertEqUInt(lblBuf, expLineInd, this->stp->getFocusInd().lineInd);
    }

    inline void assertFocus(unit::TestContext *tc,
            bool expTop, size_t expBlockInd, size_t expLineInd) {
        this->lblAssertFocus(tc, nullptr, expTop, expBlockInd, expLineInd);
    }

    // Only height is variable for a test.
    // All tests will have identical width and vertical line spacing.
    ScrollTextPaneTestCase(const char *n, uint8_t h) 
        : unit::TestCase(n), 
        stpi({ .height = h, .lineWidth = 40, .vertLineSpace = 2,}) {
    }
};

class STPTestCase1 : public ScrollTextPaneTestCase {
private:
    static STPTestCase1 ONLY_VAL;

    virtual void attemptBody(unit::TestContext *tc) override {
        // Log 3 lines.
        this->stp->log(&TC_TEXT_INFO_2, "AAA AAA AAA"); 
        this->lblAssertFocus(tc, "3L", true, 0, 0);

        this->stp->log(&TC_TEXT_INFO_2, "AAA");
        this->lblAssertFocus(tc, "4L", true, 0, 0);

        this->stp->log(&TC_TEXT_INFO_2, "AAA AAA");
        this->lblAssertFocus(tc, "5L.0", false, 2, 1);

        this->stp->scrollUp();
        this->lblAssertFocus(tc, "5L.1", true, 0, 1);

        this->stp->scrollUp();
        this->lblAssertFocus(tc, "5L.2", true, 0, 0);

        this->stp->scrollDown();
        this->lblAssertFocus(tc, "5L.3", false, 2, 0);

        // Half height
        this->stp->log(&TC_TEXT_INFO_1, "AAA AAA");
        this->lblAssertFocus(tc, "7L.0", false, 3, 1);

        this->stp->scrollDown();
        this->lblAssertFocus(tc, "7L.1", false, 3, 1);

        this->stp->scrollUp();
        this->lblAssertFocus(tc, "7L.2", true, 1, 0);
    }

    // 4 rows (16 px lines)
    STPTestCase1() : ScrollTextPaneTestCase("STP 1", 16 + (3 * 18)) {
    }
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;
};
STPTestCase1 STPTestCase1::ONLY_VAL;

class STPTestCase2 : public ScrollTextPaneTestCase {
private:
    static STPTestCase2 ONLY_VAL;

    virtual void attemptBody(unit::TestContext *tc) override {
        this->stp->scrollDown();
        this->lblAssertFocus(tc, "0L.0", true, 0, 0);

        this->stp->scrollUp();
        this->lblAssertFocus(tc, "0L.1", true, 0, 0);

        this->stp->log(&TC_TEXT_INFO_2, "AAA");
        this->lblAssertFocus(tc, "1L", true, 0, 0);

        this->stp->log(&TC_TEXT_INFO_2, "AAA AAA");
        this->lblAssertFocus(tc, "3L.0", false, 1, 1);

        this->stp->scrollUp();
        this->lblAssertFocus(tc, "3L.1", true, 1, 0);
    }

    // 1 rows (16 px lines)
    STPTestCase2() : ScrollTextPaneTestCase("STP 2", 16) {
    }
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;
};

STPTestCase2 STPTestCase2::ONLY_VAL;

const size_t SCROLL_TEXT_PANE_SUITE_LEN = 2;
static cxxutil::unit::TestCase * const 
SCROLL_TEXT_PANE_SUITE_TESTS[SCROLL_TEXT_PANE_SUITE_LEN] = {
    STPTestCase1::ONLY,
    STPTestCase2::ONLY,
};

unit::TestSuite cxxutil::gui::SCROLL_TEXT_PANE_SUITE(
        "Scroll Text Pane Suite", SCROLL_TEXT_PANE_SUITE_TESTS,
        SCROLL_TEXT_PANE_SUITE_LEN);
