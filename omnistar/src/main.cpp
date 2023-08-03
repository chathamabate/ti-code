
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

using namespace cxxutil;


// This is like a massive declaration for a single
// test...

class Test1 : public unit::TestCase {
private:
    static Test1 ONLY_VAL;

    Test1() : TestCase("Test1") {}
    // Pointers to dynamic memory...
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;

    virtual void attempt(unit::TestContext *tc) override {
        tc->info("Hello From Test1");
    }

    virtual void finally() override {
    }
};

Test1 Test1::ONLY_VAL;


class Test2 : public unit::TestCase {
private:
    static Test2 ONLY_VAL;

    Test2() : TestCase("Test2") {}
    // Pointers to dynamic memory...
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;

    virtual void attempt(unit::TestContext *tc) override {
        tc->info("Hello From Test2");
    }

    virtual void finally() override {
    }
};

Test2 Test2::ONLY_VAL;


const size_t S_LEN = 2;
unit::TestCase * const S_TS[S_LEN] = {
    Test1::ONLY,
    Test2::ONLY
};

unit::TestSuite S("My Suite", S_TS, S_LEN);

const size_t M_LEN = 2;
unit::TestSuite * const M_SS[M_LEN] = {
    &S, &S,
};

unit::TestModule M("My Module", M_SS, M_LEN);

class MyMonitor : public unit::TestMonitor {
private:
    virtual void notifyModuleStart(unit::TestModule *mod) override {
        os_PutStrFull("M: ");
        os_PutStrFull(mod->getName());
        os_NewLine();
    }

    virtual void notifyModuleEnd() override {
        os_PutStrFull("Module Over");
        os_NewLine();
    }

    virtual void notifySuiteStart(unit::TestSuite *suite) override {
        os_PutStrFull("S: ");
        os_PutStrFull(suite->getName());
        os_NewLine();
    }

    virtual void notifySuiteEnd() override {
        os_PutStrFull("Suite Over");
        os_NewLine();
    }

    virtual void notifyTestStart(unit::TestCase *test) override {
        os_PutStrFull("T: ");
        os_PutStrFull(test->getName());
        os_NewLine();
    }

    virtual void notifyTestEnd() override {
        os_PutStrFull("Test Over");
        os_NewLine();
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

    M.run(m);

    core::waitClear();

    delete m;

    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
