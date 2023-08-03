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
    class TestCase;
    class TestMonitor;
    class TestContext;

    // Test Cases should only exist as singletons
    // in static memory. Thus, they need not extend
    // SafeObject.
    class TestCase {
    private:
        const char * const name;

        // NOTE, these are not purely virtual intentionally.
        virtual void attempt(TestContext *tc);

        // Finally is always run after the test.
        // It should clean up any memory used.
        virtual void finally();
    protected:
        TestCase(const char *n);

        // NOTE: again, as test cases should only reside
        // in static memory. There destructors should
        // never be invoked by the user!

    public:
        inline const char *getName() const {
            return this->name;
        }

        void run(TestMonitor *mn);
    };

    // NOTE: All objects made dynamically from the below types
    // will reside in the testing memory channel.

    typedef uint8_t log_level_t;

    constexpr log_level_t INFO = 0;
    constexpr log_level_t WARN = 1;
    constexpr log_level_t FATAL = 2;

    class TestMonitor : public core::SafeObject {
        friend class TestCase;
        friend class TestContext;

    protected:
        virtual void notifyTestStart(TestCase *test) = 0;
        virtual void notifyTestEnd() = 0;

        // NOTE: this will never store the pointer msg!
        // If the string must be stored, it will be copied.
        virtual void log(log_level_t level, const char *msg) = 0;

    public:
        TestMonitor();
        virtual ~TestMonitor();
    };

    // NOTE: a TestContext is simply a wrapper around the
    // user defined test monitor. It provides helper functions
    // for assertions and more...
    class TestContext : public core::SafeObject {
        friend class TestCase;

    private:
        TestMonitor *mn;

        // Environment used to exit the test early if needed.
        jmp_buf *exitEnv;

        // NOTE: these are hidden so that the user can
        // never create a test context!
        TestContext(jmp_buf *jb, TestMonitor *m);
        ~TestContext();

        void log(log_level_t level, const char *msg);

    public:
        void stopTest();

        // The buffers used to log messages will never
        // be greater than this length.
        //
        // NOTE: consider changing this, as dynamically
        // sized strings are useful.
        static constexpr size_t TC_MSG_BUF_SIZE = 100;

        inline void info(const char *msg) {
            this->mn->log(INFO, msg);
        }

        inline void warn(const char *msg) {
            this->mn->log(WARN, msg);
        } 

        // NOTE: fatal will stop the test always!
        void fatal(const char *msg) {
            this->mn->log(FATAL, msg);
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
}
}
