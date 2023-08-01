#include <cstddef>
#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>

#include <string.h>


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

TestRun::TestRun() 
    : core::SafeObject(core::CXX_TEST_CHNL) {
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
    // Do something with the jump buffer I guess?
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

    char eStr[2] = { expected, '\0' };
    char aStr[2] = { actual, '\0' };

    char buf[TC_MSG_BUF_SIZE];      
    size_t bufLen = 0;

    bufLen = catLabel(buf, bufLen, TC_MSG_BUF_SIZE, label);

    // Probably a better way to do better cats.

    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, 
            "Assert equal character failutre. (ex = ");
    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, eStr);
    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, ", ac = ");
    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, aStr);
    bufLen = core::strCatSafe(buf, bufLen, TC_MSG_BUF_SIZE, ")");

    this->fatal(buf);
}













