
#include <cxxutil/gui/text_block.h>
#include <cxxutil/gui/scroll_text_pane.h>

#include "cxxutil/gui/test/centered_pane.h"
#include "cxxutil/unitapp/unit_app.h"

#include <cxxutil/core/mem.h>
#include <cxxutil/core/input.h>
#include <cxxutil/core/data.h>
#include <cxxutil/unit/unit.h>

#include <cxxutil/gui/test/text_block.h>
#include <cxxutil/gui/test/tree_pane.h>
#include <cxxutil/gui/test/scroll_text_pane.h>


#include <stdio.h>
#include <inttypes.h>

using namespace cxxutil;

class SortedCase : public unit::TestCase {
private:
    const uint64_t * const arr;
    const size_t arrLen;

    virtual void attempt(unit::TestContext *tc) override {
        tc->info("Inserting X (31)");
        tc->info("Inserting Y (128)");
        tc->info("Sorting...");
        tc->warn("Unexpected sort behavoir.");
        tc->lblAssertEqUInt("BP1", 31, 2);
            
    }

public:
    SortedCase(const char *name, const uint64_t *a, size_t al) 
        : TestCase(name), arr(a), arrLen(al) {}
};








int main(void) {    

    uint64_t A[2] = {1, 0};
    SortedCase sc("Insertion Sort Test", A, 2);

    unitapp::runUnitApp(&sc);
        
    core::MemoryTracker::ONLY->checkMemLeaks();

    return 0;
}
