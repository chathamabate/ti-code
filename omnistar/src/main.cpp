
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
        os_PutStrFull("S: ");
        os_PutStrFull(suite->getName());
        os_NewLine();
    }

    virtual void notifySuiteEnd() override {
    }

    virtual void notifyTestStart(unit::TestCase *test) override {
        os_PutStrFull("T: ");
        os_PutStrFull(test->getName());
        os_NewLine();
    }

    virtual void notifyTestEnd() override {
        core::waitClear();
    }

    virtual void log(unit::log_level_t level, const char *msg) override {
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

    return 0;
}
