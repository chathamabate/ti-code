#pragma once

#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>

namespace cxxutil {
namespace unit {

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
        friend const TestRun *runUnitTest(const unit_test_t *ut);

    private:
        bool memLeak;   // Whether or not there was a memory leak.

        core::CoreList<TestLogLine *> *logs;

        // This is the highest level ever reported to
        // the logs.
        log_level_t maxLevel;

        TestRun();

        void stopTest();

        void info(char *msg);
        void warn(char *msg);
        void fatal(char *msg);   

        inline void reportMemLeak() {
            this->memLeak = true;
        }

        void lbl_assert_true(const char *label, bool p);

        inline void assert_true(bool p) {
            this->lbl_assert_true(nullptr, p);
        }

        void lbl_assert_false(const char *label, bool p);

        inline void assert_false(bool p) {
            this->lbl_assert_false(nullptr, p);
        }

        void lbl_assert_eq_char(const char *label, char expected, char actual);

        inline void assert_eq_char(char expected, char actual) {
            this->lbl_assert_eq_char(nullptr, expected, actual);
        }

        void lbl_assert_eq_int(const char *label, int expected, int actual);

        inline void assert_eq_int(int expected, int actual) {
            this->lbl_assert_eq_int(nullptr, expected, actual);
        }

        void lbl_assert_eq_uint(const char *label, unsigned int expected, unsigned int actual);

        inline void assert_eq_uint(unsigned int expected, unsigned int actual) {
            this->lbl_assert_eq_uint(nullptr, expected, actual);
        }

        void lbl_assert_eq_str(const char *label, const char *expected, const char *actual);

        inline void assert_eq_str(const char *expected, const char *actual) {
            this->lbl_assert_eq_str(nullptr, expected, actual);
        }

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

    typedef struct {
        const char *name;
        void (*test)(TestRun *t);
    } unit_test_t;

    const TestRun *runUnitTest(const unit_test_t *ut);
}
}