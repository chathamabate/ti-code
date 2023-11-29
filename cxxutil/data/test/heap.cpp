
#include <cxxutil/data/heap.h>
#include <cxxutil/data/test/heap.h>
#include <cxxutil/unit/unit.h>

using namespace cxxutil;

static core::U24 trivKey(const uint8_t &v) {
    return v;
}

class SimpleHeapCase1 : public unit::TestCase {
private:
    static SimpleHeapCase1 ONLY_VAL;
    SimpleHeapCase1() : TestCase("Simple Heap 1") {}

    data::Heap<uint8_t> *h;

    virtual void attempt(unit::TestContext *tc) override {
        this->h = new data::Heap<uint8_t>(2, trivKey, 1);
        
        this->h->push(4);
        tc->assertEqUInt(1, this->h->getLength());

        uint8_t min = this->h->pop();
        tc->assertEqUInt(4, min);
    }

    virtual void finally() override {
        delete this->h;
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

SimpleHeapCase1 SimpleHeapCase1::ONLY_VAL;

class SimpleHeapCase2 : public unit::TestCase {
private:
    static SimpleHeapCase2 ONLY_VAL;
    SimpleHeapCase2() : TestCase("Simple Heap 2") {}

    data::Heap<uint8_t> *h;

    virtual void attempt(unit::TestContext *tc) override {
        this->h = new data::Heap<uint8_t>(2, trivKey, 1);
        
    }

    virtual void finally() override {
        delete this->h;
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

SimpleHeapCase2 SimpleHeapCase2::ONLY_VAL;

const size_t HEAP_SUITE_LEN = 2;
static unit::TestTree * const 
HEAP_SUITE_TESTS[HEAP_SUITE_LEN] = {
    SimpleHeapCase1::ONLY,
    SimpleHeapCase2::ONLY
};

static unit::TestSuite HEAP_SUITE_VAL(
        "Heap Suite", HEAP_SUITE_TESTS, HEAP_SUITE_LEN);

unit::TestTree * const cxxutil::data::HEAP_SUITE = &HEAP_SUITE_VAL;
