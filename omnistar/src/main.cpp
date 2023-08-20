
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
#include <cxxutil/gui/test/tree_pane.h>
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

    gui::TreePaneGUITest();

    return 0;
    
    MyMonitor *m = new MyMonitor();
    gui::TREE_PANE_NODE_SUITE->run(m);
    gui::TREE_PANE_SUITE->run(m);
    core::waitClear();

    delete m;

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
