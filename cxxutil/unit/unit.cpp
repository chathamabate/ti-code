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

void lblAssertTrue(const char *label, bool p) {
    if (p) {
        return;
    }

    
}



