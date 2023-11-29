
#include <cxxutil/data/heap.h>
#include <cxxutil/data/test/heap.h>
#include <cxxutil/unit/unit.h>
#include <limits.h>

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

static void pushAll(data::Heap<uint8_t> *h, const uint8_t *eles, size_t len) {
    for (size_t i = 0; i < len; i++) {
        h->push(eles[i]);
    }
}

static void assertPopAll(unit::TestContext *tc, data::Heap<uint8_t> *h, 
        const uint8_t *eles, size_t len) {
    for (size_t i = 0; i < len; i++) {
        tc->assertTrue(h->getLength() > 0);
        tc->assertEqUInt(eles[i], h->pop());
    }
}


class SimpleHeapCase2 : public unit::TestCase {
private:
    static SimpleHeapCase2 ONLY_VAL;
    SimpleHeapCase2() : TestCase("Simple Heap 2") {}

    data::Heap<uint8_t> *h;

    virtual void attempt(unit::TestContext *tc) override {
        this->h = new data::Heap<uint8_t>(2, trivKey, 1);
        
        pushAll(this->h, 
                (uint8_t[]){4, 3, 6, 2}, 4);

        assertPopAll(tc, this->h,
                (uint8_t[]){2, 3, 4, 6}, 4); 
    }

    virtual void finally() override {
        delete this->h;
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

SimpleHeapCase2 SimpleHeapCase2::ONLY_VAL;

static core::U24 revKey(const uint8_t &v) {
    return UINT24_MAX - v;
}

class SimpleHeapCase3 : public unit::TestCase {
private:
    static SimpleHeapCase3 ONLY_VAL;
    SimpleHeapCase3() : TestCase("Simple Heap 3") {}

    data::Heap<uint8_t> *h;

    virtual void attempt(unit::TestContext *tc) override {
        this->h = new data::Heap<uint8_t>(2, revKey, 3);
        
        pushAll(this->h, 
                (uint8_t[]){2, 1, 6}, 3);

        tc->assertEqUInt(6, this->h->pop());

        // 2, 1

        pushAll(this->h, 
                (uint8_t[]){4, 4, 3, 3, 2}, 5);

        // 4, 4, 3, 3, 2, 2, 1

        assertPopAll(tc, this->h,
                (uint8_t[]){4, 4, 3, 3}, 4); 
        // 2, 2, 1

        pushAll(this->h, 
                (uint8_t[]){1, 7, 1, 0}, 4);

        // 7, 2, 2, 1, 1, 1, 0
        assertPopAll(tc, this->h,
                (uint8_t[]){7, 2, 2, 1, 1, 1, 0}, 7); 
    }

    virtual void finally() override {
        delete this->h;
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

SimpleHeapCase3 SimpleHeapCase3::ONLY_VAL;


const size_t HEAP_SUITE_LEN = 3;
static unit::TestTree * const 
HEAP_SUITE_TESTS[HEAP_SUITE_LEN] = {
    SimpleHeapCase1::ONLY,
    SimpleHeapCase2::ONLY,
    SimpleHeapCase3::ONLY
};

static unit::TestSuite HEAP_SUITE_VAL(
        "Heap Suite", HEAP_SUITE_TESTS, HEAP_SUITE_LEN);

unit::TestTree * const cxxutil::data::HEAP_SUITE = &HEAP_SUITE_VAL;
