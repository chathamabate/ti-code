
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

#include "keypadc.h"
#include "sys/timers.h"
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
    virtual void notifySuiteStart(unit::TestSuite *suite) override {
        (void)suite;
        /* os_PutStrFull("S: ");
        os_PutStrFull(suite->getName());
        os_NewLine(); */
    }

    virtual void notifySuiteEnd() override {
    }

    virtual void notifyCaseStart(unit::TestCase *c) override {
        os_PutStrFull("T: ");
        os_PutStrFull(c->getName());
        os_NewLine(); 
    }

    virtual void notifyCaseEnd() override {
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
    gui::TEXT_BLOCK_SUITE.run(m);
    gui::SCROLL_TEXT_PANE_SUITE.run(m);
    core::waitClear();

    delete m;

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;

    const gui::scroll_text_pane_info_t stpi = {
        .height = 88,

        .lineWidth = GFX_LCD_WIDTH - 100 - 8, 
        .scrollBarWidth = 8,

        .bgColor = 247,

        .scrollBarFGColor = 24,
        .scrollBarBGColor = 223,

        .vertLineSpace = 2,
    };
    
    const gui::text_info_t ti = {
        .widthScale = 1,
        .heightScale = 2,
        .monospace = 0,

        .fgColor = 0,
        .bgColor = 255,
    };

    core::KeyManager *km = new core::KeyManager(1);
    km->setFocusedKeys(
            (core::cxx_key_t[]){
                core::CXX_KEY_8, 
                core::CXX_KEY_5, 
                core::CXX_KEY_Up,
                core::CXX_KEY_Down,
                core::CXX_KEY_Clear}, 5);

    km->setRepeatDelay(5);

    gui::ScrollTextPane *stp = new gui::ScrollTextPane(1, &stpi);
    stp->focus();

    const size_t len = 4;
    const char *msgs[len] = {
        "Hello, this is a very long message I am testing.",
        "I hope I can scroll through the pane like it's a word document.",
        "~",
        "I need to be able to add new line support",
    };

    for (size_t i = 0; i < len; i++) {
        stp->log(&ti, msgs[i]);
    }

    core::MemoryTracker::ONLY->setMER(core::GraphicsMemoryExitRoutine::ONLY);

    gfx_Begin();
    gfx_SetDrawBuffer();

    do {
        km->scanFocusedKeys();
        stp->update(km);

        stp->render(50, 50);
        gfx_BlitBuffer();

        delay(100);
    } while (!km->isKeyDown(core::CXX_KEY_Clear));

    gfx_End();

    core::MemoryTracker::ONLY->setMER(core::BasicMemoryExitRoutine::ONLY);

    delete stp;
    delete km;

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
