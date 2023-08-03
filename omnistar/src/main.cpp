
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

class MyTestCase : public unit::TestCase {
private:
    static MyTestCase ONLY_VAL;

    MyTestCase() : TestCase("MyTest") {}
    // Pointers to dynamic memory...
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;

    virtual void attempt(unit::TestContext *tc) override {
        tc->info("Hello");
        tc->fatal("Goodbye");
        tc->info("Don't print");
    }

    virtual void finally() override {
        os_PutStrFull("Clean-UP");
        os_NewLine();
    }
};

MyTestCase MyTestCase::ONLY_VAL;


class MyMonitor : public unit::TestMonitor {
private:
    virtual void notifyTestStart(unit::TestCase *test) override {
        os_PutStrFull("S: ");
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


class A {
public:
    ~A() {
        os_PutStrFull("Deleting A");
        os_NewLine();
    }
};

class B : public A {
public:
    virtual ~B() {
        os_PutStrFull("Deleteing B");
        os_NewLine();
    }
};

class C : public B {
public:
    virtual ~C() override {
        os_PutStrFull("Deleting C");
        os_NewLine();
    }
};



int main(void) {    
    os_ClrHome();

    core::waitClear();
    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
