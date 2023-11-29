
#include <cstdio>
#include <cxxutil/unit/unit.h>
#include <cxxutil/data/bits.h>
#include <cxxutil/data/test/bits.h>

using namespace cxxutil;

class BitVectorCase : public unit::TestCase {
private:
    const size_t inputNB;

    data::BitVector *bv;

    virtual void attempt(unit::TestContext *tc) override {
        this->bv = new data::BitVector(2, this->inputNB);

        const size_t nb = this->bv->getNumBits();
        tc->lblAssertEqUInt("Eq Bits", this->inputNB, nb);

        // Set every even bit to 1.
        for (size_t bi = 0; bi < nb; bi += 2) {
            this->bv->set(bi, true);
        }

        // Set every third bit to 0.
        for (size_t bi = 0; bi < nb; bi += 3) {
            this->bv->set(bi, false);
        }

        char lbl[20];

        for (size_t bi = 0; bi < nb; bi++) {
            sprintf(lbl, "bi %u", bi);
            if (bi % 2 == 0 && bi % 3 > 0) {
                tc->lblAssertTrue(lbl, this->bv->get(bi));
            } else {
                tc->lblAssertFalse(lbl, this->bv->get(bi));
            }
        }
    }

    virtual void finally() override {
        delete this->bv;
    }

public:
    BitVectorCase(const char *name, size_t nb) 
        : TestCase(name), inputNB(nb) {}
};

static BitVectorCase BVC1("BitVector Case 1", 1);
static BitVectorCase BVC2("BitVector Case 2", 2);
static BitVectorCase BVC3("BitVector Case 3", 7);
static BitVectorCase BVC4("BitVector Case 4", 8);
static BitVectorCase BVC5("BitVector Case 5", 13);
static BitVectorCase BVC6("BitVector Case 6", 24);
static BitVectorCase BVC7("BitVector Case 7", 400);

const size_t BIT_VECTOR_SUITE_LEN = 7;
static unit::TestTree * const
BIT_VECTOR_SUITE_TESTS[BIT_VECTOR_SUITE_LEN] = {
    &BVC1,
    &BVC2,
    &BVC3,
    &BVC4,
    &BVC5,
    &BVC6,
    &BVC7,
};

static unit::TestSuite BIT_VECTOR_SUITE_VAL(
        "BitVector Suite", BIT_VECTOR_SUITE_TESTS, BIT_VECTOR_SUITE_LEN);

class BitGridCase : public unit::TestCase {
private:
    const size_t inputRows;
    const size_t inputCols;

    data::BitGrid *bg;

    virtual void attempt(unit::TestContext *tc) override {
        this->bg = new data::BitGrid(this->inputRows, this->inputCols);

        size_t rows = this->bg->getRows();
        size_t cols = this->bg->getCols();

        tc->lblAssertEqUInt("Eq Rows", this->inputRows, rows);
        tc->lblAssertEqUInt("Eq Cols", this->inputCols, cols);

        size_t totalBits = rows * cols;

        for (size_t bi = 0; bi < totalBits; bi++) {
            size_t r = bi / cols;
            size_t c = bi % cols;

            if (bi % 2 == 0) {
                this->bg->set(r, c, true);
            }

            if (bi % 3 == 0) {
                this->bg->set(r, c, false);
            }
        }

        char buf[30];

        // Now to check.
        for (size_t bi = 0; bi < totalBits; bi++) {
            size_t r = bi / cols;
            size_t c = bi % cols;

            sprintf(buf, "(%u, %u)", r, c);

            if (bi % 2 == 0 && bi % 3 > 0) {
                tc->lblAssertTrue(buf, this->bg->get(r, c)); 
            } else {
                tc->lblAssertFalse(buf, this->bg->get(r, c)); 
            }
        }
    }

    virtual void finally() override {
        delete this->bg;
    }

public:
    BitGridCase(const char *name, size_t rs, size_t cs) 
        : TestCase(name), inputRows(rs), inputCols(cs) {}
};

static BitGridCase BGC1("BitGrid Case 1", 1, 1);
static BitGridCase BGC2("BitGrid Case 2", 5, 1);
static BitGridCase BGC3("BitGrid Case 3", 1, 5);
static BitGridCase BGC4("BitGrid Case 4", 5, 5);
static BitGridCase BGC5("BitGrid Case 5", 3, 8);
static BitGridCase BGC6("BitGrid Case 6", 23, 11);
static BitGridCase BGC7("BitGrid Case 7", 32, 32);

static const size_t BIT_GRID_SUITE_LEN = 7;
static unit::TestTree * const 
BIT_GRID_SUITE_TESTS[BIT_GRID_SUITE_LEN] = {
    &BGC1,
    &BGC2,
    &BGC3,
    &BGC4,
    &BGC5,
    &BGC6,
    &BGC7,
};

static unit::TestSuite BIT_GRID_SUITE_VAL(
        "BitGrid Suite", BIT_GRID_SUITE_TESTS, BIT_GRID_SUITE_LEN);

static const size_t BIT_SUITE_TESTS_LEN = 2;
static unit::TestTree * const 
BIT_SUITE_TESTS[BIT_SUITE_TESTS_LEN] = {
    &BIT_VECTOR_SUITE_VAL,
    &BIT_GRID_SUITE_VAL,
};

static unit::TestSuite BIT_SUITE_VAL(
        "Bit Suite", BIT_SUITE_TESTS, BIT_SUITE_TESTS_LEN);

unit::TestTree * const 
cxxutil::data::BIT_SUITE = &BIT_SUITE_VAL;




