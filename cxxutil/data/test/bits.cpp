
#include "fileioc.h"
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

class BitVectorEqCase : public unit::TestCase {
private:

    static BitVectorEqCase ONLY_VAL;

    static const size_t NUM_VECTORS = 6;
    data::BitVector *bvs[NUM_VECTORS];

    virtual void attempt(unit::TestContext *tc) override {
        bvs[0] = new data::BitVector(2, 2);
        bvs[1] = new data::BitVector(2, 2);

        bvs[2] = new data::BitVector(2, 24);
        bvs[3] = new data::BitVector(2, 24);

        bvs[4] = new data::BitVector(2, 27);
        bvs[5] = new data::BitVector(2, 27);

        tc->assertTrue(*(bvs[0]) == *(bvs[1]));
        bvs[0]->set(0, true);
        tc->assertTrue(*(bvs[0]) != *(bvs[1]));

        tc->assertTrue(*(bvs[0]) != *(bvs[2]));

        tc->assertTrue(*(bvs[2]) == *(bvs[3]));
        bvs[3]->set(23, true);
        bvs[2]->set(11, true);
        tc->assertTrue(*(bvs[2]) != *(bvs[3]));

        bvs[4]->set(25, true);
        tc->assertTrue(*(bvs[4]) != *(bvs[5]));
        bvs[4]->set(25, false);
        tc->assertTrue(*(bvs[4]) == *(bvs[5]));
    }

    virtual void finally() override {
        for (size_t i = 0; i < NUM_VECTORS; i++) {
            delete this->bvs[i];
        }
    }

    BitVectorEqCase() : unit::TestCase("BitVector Eq Case") {
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

BitVectorEqCase BitVectorEqCase::ONLY_VAL;

class BitVectorFileCase : public unit::TestCase {
private:

    static BitVectorFileCase ONLY_VAL;

    data::BitVectorFileWriter *writer;
    data::BitVectorFileReader *reader;

    data::BitVector *initVec;
    data::BitVector *resVec;


    virtual void attempt(unit::TestContext *tc) override {
        this->writer = new data::BitVectorFileWriter(2);
        this->reader = new data::BitVectorFileReader(2);

        const size_t bits = 50;
        this->initVec = new data::BitVector(2, bits);

        for (size_t i = 0; i < bits; i += 2) {
            this->initVec->set(i, true);
        }

        this->resVec = NULL;

        uint8_t handle = ti_Open("TVAR", "w");
        tc->assertTrue(handle > 0);
        bool writeRes = this->writer->write(handle, this->initVec);
        ti_Close(handle);

        tc->assertTrue(writeRes);

        handle = ti_Open("TVAR", "r");
        tc->assertTrue(handle > 0);
        bool readRes = this->reader->read(handle, &(this->resVec));
        ti_Close(handle);

        tc->assertTrue(readRes);
        tc->assertTrue(this->resVec != NULL);

        tc->assertTrue(*(this->initVec) == *(this->resVec));
    }

    virtual void finally() override {
        delete this->writer;
        delete this->reader;
        delete this->initVec;

        if (this->resVec) {
            delete this->resVec;
        }
        
        ti_Delete("TVAR");
    }

    BitVectorFileCase() : unit::TestCase("BitVector File Case") {
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

BitVectorFileCase BitVectorFileCase::ONLY_VAL;

const size_t BIT_VECTOR_SUITE_LEN = 9;
static unit::TestTree * const
BIT_VECTOR_SUITE_TESTS[BIT_VECTOR_SUITE_LEN] = {
    &BVC1,
    &BVC2,
    &BVC3,
    &BVC4,
    &BVC5,
    &BVC6,
    &BVC7,

    BitVectorEqCase::ONLY,
    BitVectorFileCase::ONLY
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

class BitGridEqCase : public unit::TestCase {
private:

    static BitGridEqCase ONLY_VAL;

    data::BitGrid *bg1;
    data::BitGrid *bg2;
    data::BitGrid *bg3;

    virtual void attempt(unit::TestContext *tc) override {
        this->bg1 = new data::BitGrid(2, 4, 17);
        this->bg2 = new data::BitGrid(2, 4, 17);
        this->bg3 = new data::BitGrid(2, 3, 17);

        tc->assertTrue(*(this->bg3) != *(this->bg2));

        tc->assertTrue(*(this->bg1) == *(this->bg2));

        this->bg1->set(2, 2, true);
        tc->assertTrue(*(this->bg1) != *(this->bg2));

        this->bg1->set(2, 2, false);
        this->bg2->set(3, 5, true);
        tc->assertTrue(*(this->bg1) != *(this->bg2));

        this->bg2->set(3, 5, false);
        tc->assertTrue(*(this->bg1) == *(this->bg2));
    }

    virtual void finally() override {
        delete this->bg3;
        delete this->bg2;
        delete this->bg1;
    }

    BitGridEqCase() : unit::TestCase("BitGrid Eq Case") {
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

BitGridEqCase BitGridEqCase::ONLY_VAL;

class BitGridFileCase : public unit::TestCase {
private:

    static BitGridFileCase ONLY_VAL;

    data::BitGridFileWriter *writer;
    data::BitGridFileReader *reader;

    data::BitGrid *initGrid;
    data::BitGrid *resGrid;


    virtual void attempt(unit::TestContext *tc) override {
        this->writer = new data::BitGridFileWriter(2);
        this->reader = new data::BitGridFileReader(2);

        const size_t rows = 5;
        const size_t cols = 11;

        this->initGrid = new data::BitGrid(2, rows, cols);

        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                if (((r * cols) + c) % 2 == 0) {
                    this->initGrid->set(r, c, true);
                }
            }
        }

        this->resGrid = NULL;

        uint8_t handle = ti_Open("TVAR", "w");
        tc->assertTrue(handle > 0);
        bool writeRes = this->writer->write(handle, this->initGrid);
        ti_Close(handle);

        tc->assertTrue(writeRes);

        handle = ti_Open("TVAR", "r");
        tc->assertTrue(handle > 0);
        bool readRes = this->reader->read(handle, &(this->resGrid));
        ti_Close(handle);

        tc->assertTrue(readRes);
        tc->assertTrue(this->resGrid != NULL);

        tc->assertTrue(*(this->initGrid) == *(this->resGrid));
    }

    virtual void finally() override {
        delete this->writer;
        delete this->reader;
        delete this->initGrid;

        if (this->resGrid) {
            delete this->resGrid;
        }
        
        ti_Delete("TVAR");
    }

    BitGridFileCase() : unit::TestCase("BitGrid File Case") {
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

BitGridFileCase BitGridFileCase::ONLY_VAL;

static const size_t BIT_GRID_SUITE_LEN = 9;
static unit::TestTree * const 
BIT_GRID_SUITE_TESTS[BIT_GRID_SUITE_LEN] = {
    &BGC1,
    &BGC2,
    &BGC3,
    &BGC4,
    &BGC5,
    &BGC6,
    &BGC7,
    BitGridEqCase::ONLY,
    BitGridFileCase::ONLY
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




