#include "ti/screen.h"
#include <setjmp.h>
#include <cstddef>
#include <cstdio>
#include <tice.h>
#include <string.h>

#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>



using namespace cxxutil::unit;

TestCase::TestCase(const char *n) : name(n) {

}

void TestCase::attempt(TestContext *tc) {
    (void)tc;
}

void TestCase::finally() {

}

void TestCase::run(TestMonitor *mn) {
    // NOTE: this is volatile, as we set its value
    // in between set and long jump.
    TestContext * volatile tc = nullptr;

    size_t memChnlBuf[core::CXX_NUM_MEM_CHNLS];
    core::MemoryTracker::ONLY->remember(memChnlBuf);
    
    jmp_buf env;
    int exited = setjmp(env); 

    if (!exited) {
        // If this is the first time set jmp returns,
        // create our test context and run our test!
        tc = new TestContext(&env, mn);

        mn->notifyTestStart(this);
        this->attempt(tc);
    } 

    // NOTE: we make it here in two situations.
    // 1) Our test runs normally and exits without
    // calling longjmp.
    // 2) Our test exits after calling longjmp.
    //
    // In both cases, we should clean up our test.

    this->finally();

    if (!core::MemoryTracker::ONLY->consistent(memChnlBuf,
                core::CXX_TEST_CHNL + 1)) {
        mn->log(FATAL, "Memory issue");
    }

    mn->notifyTestEnd();

    delete tc;
}

TestMonitor::TestMonitor() : core::SafeObject(core::CXX_TEST_CHNL) {

}

TestMonitor::~TestMonitor() {

}

TestContext::TestContext(jmp_buf *jb, TestMonitor *m) 
    : core::SafeObject(core::CXX_TEST_CHNL) {
    this->mn = m;
    this->exitEnv = jb;
}

TestContext::~TestContext() {
}

void TestContext::stopTest() {
    longjmp(*(this->exitEnv), 1);
}

static size_t catLabel(char *buf, size_t bufLen, size_t bufSize, const char *label) {
    // Only perform label cat if a label is given.
    if (!label) {
        return bufLen;
    }
    
    size_t finalLen = bufLen;

    finalLen = cxxutil::core::strCatSafe(buf, finalLen, bufSize, label); 
    finalLen = cxxutil::core::strCatSafe(buf, finalLen, bufSize, ": ");

    return finalLen;
}

void TestContext::lblAssertTrue(const char *label, bool p) {
    if (p) {
        return;
    }

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);
    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, "Assert true failure.");

    this->fatal(buf);
}

void TestContext::lblAssertFalse(const char *label, bool p) {
    if (!p) {
        return;
    }

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);
    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, "Assert false failure.");

    this->fatal(buf);
}

void TestContext::lblAssertEqChar(const char *label, char expected, char actual) {
    if (expected == actual) {
        return;
    } 

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);

    char eStr[2] = { expected, '\0' };
    char aStr[2] = { actual, '\0' };

    constexpr size_t NUM_STRS = 5;
    const char *strs[NUM_STRS] = {
        "Assert eq char failure. (ex = ", eStr, 
        ", ac = ", aStr, ")"
    };

    bufLen = core::multiStrCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, NUM_STRS, strs);
    this->fatal(buf);
}

void TestContext::lblAssertEqInt(const char *label, int expected, int actual) {
    if (expected == actual) {
        return;
    } 

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);

    // 25 characters should be more than enough to hold an integer.
    char eStr[25];
    sprintf(eStr, "%d", expected);

    char aStr[25];
    sprintf(aStr, "%d", actual);

    constexpr size_t NUM_STRS = 5;
    const char *strs[NUM_STRS] = {
        "Assert eq int failure (ex = ", eStr, 
        ", ac = ", aStr, ")"
    };

    bufLen = core::multiStrCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, NUM_STRS, strs);
    this->fatal(buf);
}

void TestContext::lblAssertEqUInt(const char *label, 
        unsigned int expected, unsigned int actual) {
    if (expected == actual) {
        return;
    } 

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);

    // 25 characters should be more than enough to hold an integer.
    char eStr[25];
    sprintf(eStr, "%u", expected);

    char aStr[25];
    sprintf(aStr, "%u", actual);

    constexpr size_t NUM_STRS = 5;
    const char *strs[NUM_STRS] = {
        "Assert eq uint failure (ex = ", eStr, 
        ", ac = ", aStr, ")"
    };

    bufLen = core::multiStrCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, NUM_STRS, strs);
    this->fatal(buf);
}

void TestContext::lblAssertEqPtr(const char *label, 
        const void *expected, const void *actual) {
    if (expected == actual) {
        return;
    } 

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);

    // 25 characters should be more than enough to hold an integer.
    char eStr[25];
    sprintf(eStr, "%p", expected);

    char aStr[25];
    sprintf(aStr, "%p", actual);

    constexpr size_t NUM_STRS = 5;
    const char *strs[NUM_STRS] = {
        "Assert eq ptr failure (ex = ", eStr, 
        ", ac = ", aStr, ")"
    };

    bufLen = core::multiStrCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, NUM_STRS, strs);
    this->fatal(buf);
}

void TestContext::lblAssertEqStr(const char *label, 
        const char *expected, const char *actual) {
    if (!strcmp(expected, actual)) {
        return;
    }
    
    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);

    constexpr size_t NUM_STRS = 5;
    const char *strs[NUM_STRS] = {
        "Assert eq str failure (ex = ", expected, 
        ", ac = ", actual, ")"
    };

    bufLen = core::multiStrCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, NUM_STRS, strs);
    this->fatal(buf);
}
