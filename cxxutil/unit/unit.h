#pragma once

#include "cdefs.h"
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
    
    virtual void attempt(unit::TestContext *tc) override {
        // Test Code....
    }

    virtual void finally() override {
        // Clean Up Code...
    }
public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

MyTestCase MyTestCase::ONLY_VAL;

//////////////////////////////////////////////////////////////
*/

namespace cxxutil { namespace unit {
    class TestTree;
    class TestCase;
    class TestSuite;

    class TestMonitor;
    class TestContext;

    class TestTree {
    private:
        const char * const name;

    protected:
        TestTree(const char *n);

    public:
        inline const char *getName() const {
            return this->name;
        }

        // NOTE: a suite holds multiple test cases.
        // A case is just one singular unit test.
        virtual bool isCase() const = 0;
        inline bool isSuite() const {
            return !(this->isCase());
        }

        virtual void run(TestMonitor *mn) = 0;

        // These only work as expected if this is a 
        // suite.
        virtual TestTree * const *getSubTests() const = 0;
        virtual size_t getSubTestsLen() const = 0;
    };

    class TestCase : public TestTree {
    private:
        // NOTE, these are not purely virtual intentionally.
        virtual void attempt(TestContext *tc);

        // Finally is always run after the test.
        // It should clean up any memory used.
        virtual void finally();
    protected:
        TestCase(const char *n);
    public:
        inline virtual bool isCase() const override {
            return true;
        }

        virtual void run(TestMonitor *mn) override;

        inline virtual TestTree * const *getSubTests() const override {
            return nullptr;
        }

        inline virtual size_t getSubTestsLen() const override {
            return 0;
        }
    };

    class TestSuite : public TestTree {
    private:
        TestTree * const * const subTests;
        const size_t subTestsLen;
    public:
        // Test suites should NEVER exist in dynamic memory.
        // They do not extend SafeObject.
        TestSuite(const char *n, TestTree * const *sts, size_t stsLen);

        inline virtual bool isCase() const override {
            return false;
        }

        virtual void run(TestMonitor *mn) override;

        inline virtual TestTree * const *getSubTests() const override {
            return this->subTests;
        }

        inline virtual size_t getSubTestsLen() const override {
            return this->subTestsLen; 
        }
    };

    // NOTE: All objects made dynamically from the below types
    // will reside in the testing memory channel.

    typedef uint8_t log_level_t;

    constexpr log_level_t INFO = 0;
    constexpr log_level_t WARN = 1;
    constexpr log_level_t FATAL = 2;

    class TestMonitor : public core::SafeObject {
        friend class TestCase;
        friend class TestSuite;

        friend class TestContext;

    protected:
        // NOTE: the defualt versions of all these virtual
        // functions will do nothing.
        virtual void notifyCaseStart(TestCase *c);
        virtual void notifyCaseEnd();

        virtual void notifySuiteStart(TestSuite *suite);
        virtual void notifySuiteEnd();

        // NOTE: this will never store the pointer msg!
        // If the string must be stored, it will be copied.
        virtual void log(log_level_t level, const char *msg);

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
}}
