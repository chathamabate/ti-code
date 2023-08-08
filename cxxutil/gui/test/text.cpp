#include "./text.h"
#include "cxxutil/core/mem.h"
#include "cxxutil/unit/unit.h"
#include "graphx.h"

#include <cxxutil/gui/text.h>

using namespace cxxutil;

class SimpleTB1 : public unit::TestCase {
private:
    static SimpleTB1 ONLY_VAL;

    SimpleTB1() : TestCase("Simple TB 1") {}

    gui::TextBlock *tb;
public:
    static constexpr unit::TestCase *ONLY = &ONLY_VAL;

    virtual void attempt(unit::TestContext *tc) override {
        gfx_SetTextScale(1, 1);

        tb = new gui::TextBlock(1, "AAA AAA AAAAA", 32);
        const core::SafeArray<const core::SafeArray<char> *> *lines = tb->getLines();
        
        for (size_t i = 0; i < lines->getLen(); i++) {
            tc->info(lines->get(i)->getArr());
        }
    }

    virtual void finally() override {
        delete tb;
    }
};

SimpleTB1 SimpleTB1::ONLY_VAL;

const size_t TEXT_SUITE_LEN = 1;
static cxxutil::unit::TestCase * const TEXT_SUITE_TESTS[TEXT_SUITE_LEN] = {
    SimpleTB1::ONLY
};

unit::TestSuite cxxutil::gui::TEXT_SUITE("Text Suite", TEXT_SUITE_TESTS, TEXT_SUITE_LEN);
