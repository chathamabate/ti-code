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

TestLogLine::TestLogLine(log_level_t l, const char *m)
    : core::SafeObject(core::CXX_TEST_CHNL) {
    this->level = l;

    size_t len = strlen(m);   
    
    this->msg = new core::SafeArray<char>(core::CXX_TEST_CHNL, len + 1);

    // Copy given string into dynamic memory.
    strcpy(this->msg->getArr(), m);
}

TestLogLine::~TestLogLine() {
    delete this->msg;
}

TestRun::TestRun(const unit_test_t *ut) 
    : core::SafeObject(core::CXX_TEST_CHNL) {
    this->parentTest = ut;
    this->memLeak = false;
    this->logs = new core::CoreList<TestLogLine *>(core::CXX_TEST_CHNL);
    this->maxLevel = INFO;
}

TestRun::~TestRun() {
    delete this->logs;
}

TestContext::TestContext(jmp_buf *jb, TestRun *tr) 
    : core::SafeObject(core::CXX_TEST_CHNL) {
    this->exitEnv = jb;
    this->testRun = tr;
}

TestContext::~TestContext() {
    // Don't think I really need to do anything here. 
    // Remember that test run persists outside of
    // this individual context.
}

void TestContext::stopTest() {
    longjmp(*(this->exitEnv), 1);
}

void TestContext::log(log_level_t level, const char *msg) {
    const char *logMsg = msg;

    // If the given message is too long,
    // we copy it into a temporary static buffer
    // with a constant max size.
    char tempBuf[TC_MSG_BUF_SIZE];
    if (strlen(msg) >= TC_MSG_BUF_SIZE) {
        memcpy(tempBuf, msg, TC_MSG_BUF_SIZE - 1);
        tempBuf[TC_MSG_BUF_SIZE - 1] = '\0';

        logMsg = tempBuf;
    }

    TestLogLine *logLine = new TestLogLine(level, logMsg);

    this->testRun->logs->add(logLine);

    if (level > this->testRun->maxLevel) {
        this->testRun->maxLevel = level;
    }
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

const TestRun *cxxutil::unit::runUnitTest(const unit_test_t *ut) {
    // The value of tr will not change, thus
    // it does not need to be marked volatile.
    TestRun * const tr = new TestRun(ut);

    // We will create our test context after calling setjmp,
    // so that we have access to the jmp buffer.
    //
    // We must make sure that this variable is not stored in a
    // register!
    TestContext * volatile tc = nullptr;

    // NOTE: Maybe I should ask about this, but my assumption
    // is that the setjmp env will only potentially hold the
    // values of local variables in this function.
    //
    // I do not believe I need to worry about global variables.
    
    jmp_buf env;
    int exited = setjmp(env); 

    os_PutStrFull("Here");
    os_NewLine();

    if (!exited) {
        // If this is the first time set jmp returns,
        // create our test context and run our test!
        tc = new TestContext(&env, tr);
        ut->test(tc);
    } 

    // NOTE: we make it here in two situations.
    // 1) Our test runs normally and exits without
    // calling longjmp.
    // 2) Our test exits after calling longjmp.
    //
    // In both cases, we should delete the TestContext
    // and return the TestRun.

    if (core::memLeaks(core::CXX_TEST_CHNL + 1)) {
        tr->memLeak = true;
    }

    delete tc;
    return tr;
}

