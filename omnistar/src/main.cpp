
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

#include "keypadc.h"
#include "ti/getcsc.h"
#include "ti/screen.h"
#include <cstddef>
#include <cstdint>
#include <stdlib.h>
#include <tice.h>
#include <stdio.h>
#include <new>
#include <graphx.h>
#include <string.h>

#include <cutil/keys.h>
#include <cxxutil/core/mem.h>
#include <cxxutil/core/input.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>

#include <cxxutil/gui/test/text_block.h>
#include <cxxutil/gui/test/scroll_text_pane.h>

using namespace cxxutil;


class MyMonitor : public unit::TestMonitor {
private:
    virtual void notifyModuleStart(unit::TestModule *mod) override {
        os_PutStrFull("M: ");
        os_PutStrFull(mod->getName());
        os_NewLine();
    }

    virtual void notifyModuleEnd() override {
    }

    virtual void notifySuiteStart(unit::TestSuite *suite) override {
        (void)suite;
        /* os_PutStrFull("S: ");
        os_PutStrFull(suite->getName());
        os_NewLine(); */
    }

    virtual void notifySuiteEnd() override {
    }

    virtual void notifyTestStart(unit::TestCase *test) override {
        os_PutStrFull("T: ");
        os_PutStrFull(test->getName());
        os_NewLine(); 
    }

    virtual void notifyTestEnd() override {
        os_PutStrFull("Test Ended");
        os_NewLine();

        core::waitClear();
    }


    virtual void log(unit::log_level_t level, const char *msg) override {
        (void)level;
        os_PutStrFull(msg);
        os_NewLine();
    }

public:
    MyMonitor() {}
    virtual ~MyMonitor() override {}
};

int main(void) {    
    /*
    
    MyMonitor *m = new MyMonitor();
    // gui::TEXT_BLOCK_SUITE.run(m);
    gui::SCROLL_TEXT_PANE_SUITE.run(m);
    core::waitClear();

    delete m;

    core::MemoryTracker::ONLY->checkMemLeaks();
    core::waitClear();
    */

    gui::scroll_text_pane_info_t stpi = {
        .height = 120,

        .lineWidth = 200,
        .scrollBarWidth = 8,

        .bgColor = 247,

        .scrollBarFGColor = 24,
        .scrollBarBGColor = 223,

        .vertLineSpace = 2,
    };


    gui::ScrollTextPane *stp = new gui::ScrollTextPane(1, &stpi);
    
    core::MemoryTracker::ONLY->setMER(core::GraphicsMemoryExitRoutine::ONLY);

    gfx_Begin();

    gfx_SetDrawBuffer();
    
    gfx_FillScreen(255);
    stp->render(50, 50);

    gfx_SetClipRegion(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT);
    // gfx_SetTextConfig(gfx_text_clip);

    // HMMM no more text clipping I guess... what instead????
    gfx_SetTextScale(1, 1);
    gfx_SetTextFGColor(0);
    gfx_PrintStringXY("Hello", 10, 10);

    gfx_SwapDraw();
    gfx_Wait();
    
    core::waitClear();

    gfx_End();

    delete stp;

    core::MemoryTracker::ONLY->checkMemLeaks();
    core::waitClear();

    return 0;
}
