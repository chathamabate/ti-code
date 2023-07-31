#pragma once

#include <cxxutil/core/mem.h>
#include <cxxutil/core/data.h>

namespace cxxutil {
namespace unit {

    // NOTE: All objects made dynamically from the below types
    // will reside in the testing memory channel.

    enum class TestLogLevel {
        INFO, WARN, FATAL
    };

    class TestLogLine : public core::SafeObject {
    private:
        TestLogLevel level;

        // The message of a TestLogLine will always be a unique
        // copy which resides in dynamic memory. For this reason,
        // we use a SafeArray to represent the string.
        core::SafeArray<char> *msg;  
    
    public:
        TestLogLine(TestLogLevel l, const char *m);
        ~TestLogLine();

        inline TestLogLevel getLevel() {
            return this->level;
        }

        inline const char *getMsg() {
            return this->msg->getArr();
        }
    };

    class TestContext :  public core::SafeObject {
    private:
         core::CoreList<TestLogLine *> logs;
    public:
        TestContext();
        ~TestContext();

        // These do not stop the test!
        void info(char *msg);
        void warn(char *msg);

        // Fatal will stop the test!
        void fatal(char *msg);   

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
    };

}
}