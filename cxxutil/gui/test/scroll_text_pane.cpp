
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

    void lblAssertEqInd(unit::TestContext *tc, const char *lbl, 
            size_t expBlockInd, size_t expLineInd, gui::tp_index_t acInd) {
        char lblBuf[STPTC_LBL_BUF_SIZE]; 

        const char *blockIndLblParts[2] = {
            lbl, ".blockInd"
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, blockIndLblParts);
        tc->lblAssertEqUInt(lblBuf, expBlockInd, acInd.blockInd);

        const char *lineIndLblParts[2] = {
            lbl, ".lineInd"
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, lineIndLblParts);
        tc->lblAssertEqUInt(lblBuf, expLineInd, acInd.lineInd);
    }

    void lblAssertFocus(unit::TestContext *tc, const char *lbl,
            bool expTop, size_t expBlockInd, size_t expLineInd) {
        char lblBuf[STPTC_LBL_BUF_SIZE]; 

        const char *topLblParts[2] = {
            lbl, ".top" 
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, topLblParts);
        tc->lblAssertTrue(lblBuf, expTop == this->stp->getTop());

        this->lblAssertEqInd(tc, lbl, expBlockInd, expLineInd, this->stp->getViewInd());
    }

    void lblAssertTotalHeight(unit::TestContext *tc, const char *lbl,
            uint24_t expTotalHeight, uint24_t expTotalViewHeight) {
        char lblBuf[STPTC_LBL_BUF_SIZE]; 

        const char *thLblParts[2] = {
            lbl, ".th" 
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, thLblParts);
        tc->lblAssertEqUInt(lblBuf, expTotalHeight, this->stp->getTotalHeight());

        const char *tvhLblParts[2] = {
            lbl, ".tvh"
        };

        core::multiStrCatSafe(lblBuf, 0, STPTC_LBL_BUF_SIZE, 2, tvhLblParts);
        tc->lblAssertEqUInt(lblBuf, expTotalViewHeight, this->stp->getTotalViewHeight());
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

class STPTestNext : public ScrollTextPaneTestCase {
private:
    static STPTestNext ONLY_VAL;

    virtual void attemptBody(unit::TestContext *tc) override {
        // NOTE: for testing nextUp/Down, the given index must be valid!
        gui::tp_index_t ind = { .blockInd = 0, .lineInd = 0 };

        this->stp->log(&TC_TEXT_INFO_1, "AAA");

        tc->lblAssertFalse("1L.0", this->stp->nextUp(ind, &ind));
        this->lblAssertEqInd(tc, "1L.1", 0, 0, ind);

        tc->lblAssertFalse("1L.2", this->stp->nextDown(ind, &ind));
        this->lblAssertEqInd(tc, "1L.3", 0, 0, ind);

        this->stp->log(&TC_TEXT_INFO_1, "AAA AAA");
        
        tc->lblAssertTrue("3L.0", this->stp->nextDown(ind, &ind)); 
        tc->lblAssertTrue("3L.1", this->stp->nextDown(ind, &ind)); 
        tc->lblAssertFalse("3L.2", this->stp->nextDown(ind, &ind)); 
        this->lblAssertEqInd(tc, "3L.3", 1, 1, ind);

        tc->lblAssertTrue("3L.4", this->stp->nextUp(ind, &ind)); 
        this->lblAssertEqInd(tc, "3L.5", 1, 0, ind);

        tc->lblAssertTrue("3L.6", this->stp->nextUp(ind, &ind)); 
        tc->lblAssertFalse("3L.7", this->stp->nextUp(ind, &ind)); 
        this->lblAssertEqInd(tc, "3L.8", 0, 0, ind);
    }

    // 2 Rows 8px, shouldn't matter though for this test.
    STPTestNext() : ScrollTextPaneTestCase("STP Next", 18) {
    }
public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

STPTestNext STPTestNext::ONLY_VAL;

class STPTestGeneral : public ScrollTextPaneTestCase {
private:
    static STPTestGeneral ONLY_VAL;

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
    STPTestGeneral() : ScrollTextPaneTestCase("STP General", 16 + (3 * 18)) {
    }
public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};
STPTestGeneral STPTestGeneral::ONLY_VAL;

class STPTest1Row : public ScrollTextPaneTestCase {
private:
    static STPTest1Row ONLY_VAL;

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
    STPTest1Row() : ScrollTextPaneTestCase("STP 1 Row", 16) {
    }
public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

STPTest1Row STPTest1Row::ONLY_VAL;

class STPTestViewHeight : public ScrollTextPaneTestCase {
private:
    static STPTestViewHeight ONLY_VAL;

    virtual void attemptBody(unit::TestContext *tc) override {
        this->lblAssertTotalHeight(tc, "0L", 0, 0);

        this->stp->log(&TC_TEXT_INFO_1, "A");
        this->lblAssertTotalHeight(tc, "1L", 8, 0);

        this->stp->log(&TC_TEXT_INFO_1, "A");
        this->lblAssertTotalHeight(tc, "2L", 18, 0);

        this->stp->log(&TC_TEXT_INFO_1, "A");
        this->lblAssertTotalHeight(tc, "3L", 28, 28);

        this->stp->log(&TC_TEXT_INFO_1, "AAA AAA");
        this->lblAssertTotalHeight(tc, "5L.0", 48, 48);

        this->stp->scrollUp();
        this->lblAssertTotalHeight(tc, "5L.1", 48, 20);

        this->stp->scrollUp();
        this->lblAssertTotalHeight(tc, "5L.2", 48, 10);

        this->stp->scrollUp();
        this->lblAssertTotalHeight(tc, "5L.3", 48, 0);

        this->stp->scrollDown();
        this->lblAssertTotalHeight(tc, "5L.4", 48, 28);

        this->stp->scrollDown();
        this->lblAssertTotalHeight(tc, "5L.5", 48, 38);

        this->stp->scrollDown();
        this->lblAssertTotalHeight(tc, "5L.6", 48, 48);
    }

    // 2 rows 8px
    STPTestViewHeight() : ScrollTextPaneTestCase("STP View Height", 18) {
    }
public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

STPTestViewHeight STPTestViewHeight::ONLY_VAL;

const size_t SCROLL_TEXT_PANE_SUITE_LEN = 4;
static cxxutil::unit::TestTree * const 
SCROLL_TEXT_PANE_SUITE_TESTS[SCROLL_TEXT_PANE_SUITE_LEN] = {
    STPTestNext::ONLY,

    STPTestGeneral::ONLY,
    STPTest1Row::ONLY,

    STPTestViewHeight::ONLY,
};

unit::TestSuite cxxutil::gui::SCROLL_TEXT_PANE_SUITE(
        "Scroll Text Pane Suite", SCROLL_TEXT_PANE_SUITE_TESTS,
        SCROLL_TEXT_PANE_SUITE_LEN);
