
#include "cxxutil/core/mem.h"
#include "fileioc.h"
#include <cstdio>
#include <cstdlib>
#include <cxxutil/unit/unit.h>

#include <cxxutil/data/file.h>
#include <cxxutil/data/test/file.h>

// Here we want to test the Array readers/writers.

using namespace cxxutil;

class ShallowArrayCase : public unit::TestCase {
private:
    static ShallowArrayCase ONLY_VAL;
    ShallowArrayCase() : TestCase("Shallow Array Case") {}

    core::SafeArray<int> *initArr;
    core::SafeArray<int> *resArr;

    data::ShallowArrayFileWriter<int> *writer;
    data::ShallowArrayFileReader<int> *reader;

    uint8_t handle;

    virtual void attempt(unit::TestContext *tc) override {
        srand(123);
        const size_t len = 50;

        this->initArr = new core::SafeArray<int>(2, len);
        this->resArr = NULL;
        this->writer = new data::ShallowArrayFileWriter<int>(2);
        this->reader = new data::ShallowArrayFileReader<int>(2);

        this->handle = 0;

        for (size_t i = 0; i < len; i++) {
            this->initArr->set(i, rand());
        }

        this->handle = ti_Open("TVAR", "w");
        tc->assertTrue(this->handle > 0);
        bool res = this->writer->write(this->handle, this->initArr);
        ti_Close(this->handle);

        tc->assertTrue(res);

        this->handle = ti_Open("TVAR", "r");
        tc->assertTrue(this->handle > 0);
        res = this->reader->read(this->handle, &(this->resArr));
        ti_Close(this->handle);

        tc->assertTrue(res);
        
        tc->assertTrue(this->resArr != NULL);
        tc->assertEqUInt(len, this->resArr->getLen());

        for (size_t i = 0; i < len; i++) {
            tc->assertEqInt(this->initArr->get(i), this->resArr->get(i));
        }
    }

    virtual void finally() override {
        delete this->reader;
        delete this->writer;

        if (this->resArr) {
            delete this->resArr;
        }

        delete this->initArr;

        ti_Delete("TVAR");
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

ShallowArrayCase ShallowArrayCase::ONLY_VAL;

// Now for Deep Case.

class BoxInt : public core::SafeObject {
private:
    const int value;
public:
    BoxInt(uint8_t chnl, int v) 
        : core::SafeObject(chnl), value(v) {
    }

    inline int getValue() const {
        return this->value;
    }
};

class BoxIntWriter : public data::FileWriter<BoxInt *> {
public:
    BoxIntWriter(uint8_t chnl) : data::FileWriter<BoxInt *>(chnl) {
    }

    virtual ~BoxIntWriter() {
    }

    virtual bool write(uint8_t handle, BoxInt *element) {
        int value = element->getValue();

        size_t res = ti_Write(&value, sizeof(int), 1, handle);

        if (res != 1) {
            return false;
        }

        return true;
    }
};

class BoxIntReader : public data::FileReader<BoxInt *> {
public:
    BoxIntReader(uint8_t chnl) : data::FileReader<BoxInt *>(chnl) {
    }

    virtual ~BoxIntReader() {
    }

    virtual bool read(uint8_t handle, BoxInt **dest) {
        int value;
        size_t res = ti_Read(&value, sizeof(int), 1, handle);

        if (res != 1) {
            return false;
        }

        BoxInt *bi = new BoxInt(2, value);
        *dest = bi;

        return true;
    }
};

class DeepArrayCase : public unit::TestCase {
private:
    static DeepArrayCase ONLY_VAL;
    DeepArrayCase() : TestCase("Deep Array Case") {}

    BoxIntWriter *biWriter;
    BoxIntReader *biReader;

    data::DeepArrayFileWriter<BoxInt *> *writer;
    data::DeepArrayFileReader<BoxInt *> *reader;

    core::SafeArray<BoxInt *> *initArr;
    core::SafeArray<BoxInt *> *resArr;

    uint8_t handle;

    virtual void attempt(unit::TestContext *tc) override {
        this->biWriter = new BoxIntWriter(2);
        this->biReader = new BoxIntReader(2);

        this->writer = new data::DeepArrayFileWriter<BoxInt *>(
                2, this->biWriter);

        this->reader = new data::DeepArrayFileReader<BoxInt *>(
                2, this->biReader);

        const size_t len = 50;
        srand(342342);

        this->initArr = new core::SafeArray<BoxInt *>(2, len);

        for (size_t i = 0; i < len; i++) {
            this->initArr->set(i, new BoxInt(2, rand()));
        }

        this->resArr = NULL;

        // Perform our write.
        this->handle = ti_Open("TVAR", "w");
        tc->assertTrue(this->handle > 0);
        bool res = this->writer->write(this->handle, this->initArr);
        ti_Close(this->handle);

        tc->assertTrue(res);

        // Perform our read.
        this->handle = ti_Open("TVAR", "r"); 
        tc->assertTrue(this->handle > 0);
        res = this->reader->read(this->handle, &(this->resArr));
        ti_Close(this->handle);

        tc->assertTrue(res);

        tc->assertTrue(this->resArr != NULL);
        tc->assertEqUInt(len, this->resArr->getLen());

        for (size_t i = 0; i < len; i++) {
            tc->assertEqInt(this->initArr->get(i)->getValue(), 
                    this->resArr->get(i)->getValue());
        }
    }

    virtual void finally() override {
        delete this->writer;
        delete this->reader;

        delete this->biWriter;
        delete this->biReader;

        for (size_t i = 0; i < this->initArr->getLen(); i++) {
            delete this->initArr->get(i);
        }

        delete this->initArr;

        if (this->resArr) {
            for (size_t i = 0; i < this->resArr->getLen(); i++) {
                delete this->resArr->get(i);
            }

            delete this->resArr;
        }

        ti_Delete("TVAR");
    }

public:
    static constexpr unit::TestTree *ONLY = &ONLY_VAL;
};

DeepArrayCase DeepArrayCase::ONLY_VAL;


static const size_t FILE_SUITE_TESTS_LEN = 2;
static unit::TestTree * const
FILE_SUTIE_TESTS[FILE_SUITE_TESTS_LEN] = {
    ShallowArrayCase::ONLY,
    DeepArrayCase::ONLY
};

static unit::TestSuite FILE_SUITE_VALUE(
        "File Suite", FILE_SUTIE_TESTS, FILE_SUITE_TESTS_LEN);

unit::TestTree * const cxxutil::data::FILE_SUITE = &FILE_SUITE_VALUE;
