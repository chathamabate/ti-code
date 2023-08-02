#pragma once

#include <setjmp.h>

#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>

/*
//////////////////// TestCase Template //////////////////////
// Here is an example TestCase.
// Always refer to the Test Case using 
// the ONLY pointer.

class MyTestCase : public unit::TestCase {
private:
    static MyTestCase ONLY_VAL;
    MyTestCase() : TestCase("MyTest") {}

    // Pointers to dynamic memory...
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;

    virtual void attempt(unit::TestContext *tc) override {
        // Test Code....
    }

    virtual void finally() override {
        // Clean Up Code...
    }
};

MyTestCase MyTestCase::ONLY_VAL;

//////////////////////////////////////////////////////////////
*/

namespace cxxutil {
namespace unit {

    class TestLogLine;
    class TestRun;
    class TestContext;
    class TestCase;

    // Test Cases should only exist as singletons
    // in static memory. Thus, they need not extend
    // SafeObject.
    class TestCase {
    private:
        const char * const name;

    protected:
        TestCase(const char *n);

    public:
        inline const char *getName() const {
            return this->name;
        }

        // NOTE, these are not purely virtual intentionally.

        virtual void attempt(TestContext *tc);

        // Finally is always run after the test.
        // It should clean up any memory used.
        virtual void finally();
    };

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

        inline log_level_t getLevel() const {
            return this->level;
        }

        inline const char *getMsg() const {
            return this->msg->getArr();
        }
    };

    // When a test completes, this is what will
    // be returned.
    class TestRun : public core::SafeObject {
        friend class TestContext;        friend const TestRun *runUnitTest(TestCase * const ut);

    private:
        TestCase * const parentTest;

        bool memLeak;   // Whether or not there was a memory leak.

        core::CoreList<TestLogLine *> *logs;

        // This is the highest level ever reported to
        // the logs.
        log_level_t maxLevel;

        TestRun(TestCase *ut);

    public:
        ~TestRun();

        inline TestCase *getParentTest() const {
            return this->parentTest;
        }

        inline bool getMemLeak() const {
            return this->memLeak;
        }

        inline const core::CoreList<TestLogLine *> *getLogs() const {
            return this->logs;
        }

        inline log_level_t getMaxLevel() const {            
            return this->maxLevel;
        }
    };

    class TestContext : public core::SafeObject {
        friend const TestRun *runUnitTest(TestCase * const ut);
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
        static constexpr size_t TC_MSG_BUF_SIZE = 100;

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

        // NOTE: A failed assertion will log a fatal message
        // and stop the test!
        
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

        void lblAssertEqPtr(const char *label, const void *expected, const void *actual);
        inline void assertEqPtr(const void *expected, const void *actual) {
            this->lblAssertEqPtr(nullptr, expected, actual);
        }

        void lblAssertEqStr(const char *label, const char *expected, const char *actual);
        inline void assertEqStr(const char *expected, const char *actual) {
            this->lblAssertEqStr(nullptr, expected, actual);
        }


    };

    const TestRun *runUnitTest(TestCase * const ut);
}
}
