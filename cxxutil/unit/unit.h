#pragma once

#include <setjmp.h>

#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>

namespace cxxutil {
namespace unit {

    class TestLogLine;
    class TestRun;
    class TestContext;

    // I don't think unit test should be created dynamically.
    // I designed this structure so that the user would create
    // static constant tests only.
    typedef struct {
        const char * const name;
        void (* const test)(TestContext *tc);
    } unit_test_t;

    // NOTE: All objects made dynamically from the below types
    // will reside in the testing memory channel.

    typedef uint8_t log_level_t;

    constexpr log_level_t INFO = 0;
    constexpr log_level_t WARN = 1;
    constexpr log_level_t FATAL = 2;

    class TestLogLine : public core::SafeObject {
    private:
        log_level_t level;

        // The message of a TestLogLine will always be a unique
        // copy which resides in dynamic memory. For this reason,
        // we use a SafeArray to represent the string.
        core::SafeArray<char> *msg;  
    
    public:
        TestLogLine(log_level_t l, const char *m);
        ~TestLogLine();

        inline log_level_t getLevel() {
            return this->level;
        }

        inline const char *getMsg() {
            return this->msg->getArr();
        }
    };

    // When a test completes, this is what will
    // be returned.
    class TestRun : public core::SafeObject {
        friend class TestContext;        
        friend const TestRun *runUnitTest(const unit_test_t *ut);

    private:
        bool memLeak;   // Whether or not there was a memory leak.

        core::CoreList<TestLogLine *> *logs;

        // This is the highest level ever reported to
        // the logs.
        log_level_t maxLevel;

        TestRun();

    public:
        ~TestRun();

        inline bool getMemLeak() {
            return this->memLeak;
        }

        inline const core::CoreList<TestLogLine *> *getLogs() {
            return this->logs;
        }

        inline log_level_t getMaxLevel() {            
            return this->maxLevel;
        }
    };

    class TestContext : public core::SafeObject {
        friend const TestRun *runUnitTest(const unit_test_t *ut);
    private:
        TestRun *testRun;

        // Environment used to exit the test early if needed.
        jmp_buf *exitEnv;

        TestContext(jmp_buf *jb, TestRun *tr);
        ~TestContext();

        void log(log_level_t level, const char *msg);

    public:
        void stopTest();

        // The buffers used to log messages will never
        // be greater than this length.
        static constexpr uint16_t TC_MSG_BUF_SIZE = 100;

        inline void info(const char *msg) {
            this->log(INFO, msg);
        }

        inline void warn(const char *msg) {
            this->log(WARN, msg);
        } 

        // NOTE: fatal will stop the test always!
        void fatal(const char *msg) {
            this->log(FATAL, msg);
            this->stopTest();
        }

        inline void reportMemLeak() {
            this->testRun->memLeak = true;
        }

        void lblAssertTrue(const char *label, bool p);
        inline void assertTrue(bool p) {
            this->lblAssertTrue(nullptr, p);
        }

        void lblAssertFalse(const char *label, bool p);
        inline void assertFalse(bool p) {
            this->lblAssertFalse(nullptr, p);
        }

        void lblAssertEqChar(const char *label, char expected, char actual);
        inline void assertEqChar(char expected, char actual) {
            this->lblAssertEqChar(nullptr, expected, actual);
        }

        void lblAssertEqInt(const char *label, int expected, int actual);
        inline void assertEqInt(int expected, int actual) {
            this->lblAssertEqInt(nullptr, expected, actual);
        }

        void lblAssertEqUInt(const char *label, unsigned int expected, unsigned int actual);
        inline void assertEqUInt(unsigned int expected, unsigned int actual) {
            this->lblAssertEqUInt(nullptr, expected, actual);
        }

        void lblAssertEqStr(const char *label, const char *expected, const char *actual);
        inline void assertEqStr(const char *expected, const char *actual) {
            this->lblAssertEqStr(nullptr, expected, actual);
        }
    };

    const TestRun *runUnitTest(const unit_test_t *ut);
}
}
