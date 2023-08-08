
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
        /*
        gfx_SetTextScale(1, 2);

        size_t len = strlen(msg);
        core::SafeArray<char> *newMsg = 
            new core::SafeArray<char>(core::CXX_TEST_CHNL, len + 1);

        for (size_t i = 0; i <  len; i++) {
            if (msg[i] == ' ') {
                newMsg->set(i, '_');
            } else {
                newMsg->set(i, msg[i]);
            }
        }

        gfx_PrintStringXY(newMsg->getArr(), 0, l * 16);
        l++;

        delete newMsg;
        */
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

    MyMonitor *m = new MyMonitor();
    gui::TEXT_SUITE.run(m);

    core::waitClear();

    gfx_End();*/

    return 0;
}
