
#include "cxxutil/gui/text.h"
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

#include <cxxutil/gui/test/text.h>

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
    os_ClrHome();
    
    MyMonitor *m = new MyMonitor();
    gui::TEXT_SUITE.run(m);

    core::waitClear();
    
    
    /*
    core::MemoryTracker::ONLY->setMER(core::GraphicsMemoryExitRoutine::ONLY);

    gfx_Begin();
    gfx_SetDrawBuffer();

    gfx_FillScreen(10);

    gfx_SetColor(255);
    gfx_FillRectangle(0, 0, 80, GFX_LCD_HEIGHT);
 
    gfx_SwapDraw();
    gfx_Wait();
    gfx_BlitScreen(); // Copy screen to other buffer.
    
    gfx_SetTextScale(1, 2);
    gui::TextBlock *tb = new gui::TextBlock(1, "", 80);

    for (size_t r = 0; r < tb->getLines()->getLen(); r++) {
        gfx_PrintStringXY(tb->getLines()->get(r)->getArr(), 0, r * 16);
    }

    gfx_SwapDraw();
    gfx_Wait();

    core::waitClear();

    gfx_End();
    */

    return 0;
}
