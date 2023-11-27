
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
        this->bv = new data::BitVector(this->inputNB);

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
        delete bv;
    }

public:
    BitVectorCase(const char *name, size_t nb) 
        : TestCase(name), inputNB(nb) {}
};

static BitVectorCase BVC1("Bit Vector Case 1", 1);
static BitVectorCase BVC2("Bit Vector Case 2", 2);
static BitVectorCase BVC3("Bit Vector Case 3", 7);
static BitVectorCase BVC4("Bit Vector Case 4", 8);
static BitVectorCase BVC5("Bit Vector Case 5", 13);
static BitVectorCase BVC6("Bit Vector Case 6", 24);
static BitVectorCase BVC7("Bit Vector Case 7", 100);

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
        "Bit Vector Suite", BIT_VECTOR_SUITE_TESTS, BIT_VECTOR_SUITE_LEN);

unit::TestTree * const 
cxxutil::data::BIT_VECTOR_SUITE = &BIT_VECTOR_SUITE_VAL;
