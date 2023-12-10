
#include "./bits.h"
#include "cxxutil/core/mem.h"
#include "fileioc.h"
#include <cxxutil/data/file.h>

using namespace cxxutil::data;

BitVector::BitVector(uint8_t chnl, size_t bs, core::SafeArray<uint8_t> *v) 
    : core::SafeObject(chnl), bits(bs), vector(v) {
}

BitVector::BitVector(size_t bs) : BitVector(core::CXX_DEF_CHNL, bs) {
}

BitVector::BitVector(uint8_t chnl, size_t bs) 
    : core::SafeObject(chnl), bits(bs) {

    size_t len = bs / 8;

    if (bs % 8 > 0) {
        len++;
    }

    this->vector = new core::SafeArray<uint8_t>(chnl, len);

    // 0 out the vector always!
    for (size_t byte = 0; byte < len; byte++) {
        this->vector->set(byte, 0);
    }
}

BitVector::~BitVector() {
    delete this->vector;
}

void BitVector::set(size_t bit, bool value) {
    const size_t byteIndex = bit / 8;
    const size_t bitOffset = bit % 8;

    uint8_t byte = this->vector->get(byteIndex);

    if (value) {
        byte |= 1 << bitOffset;
    } else {
        byte &= ~(1 << bitOffset);
    }

    this->vector->set(byteIndex, byte);
}

bool BitVector::operator==(const BitVector &o) const {
    if (this->bits != o.bits) {
        return false;
    }

    for (size_t i = 0; i < this->vector->getLen(); i++) {
        if (this->vector->get(i) != o.vector->get(i)) {
            return false;
        }
    }

    return true;
}

BitGrid::BitGrid(size_t rs, size_t cs) 
    : BitGrid(core::CXX_DEF_CHNL, rs, cs) {
}

BitGrid::BitGrid(uint8_t chnl, size_t rs, size_t cs) 
    : core::SafeObject(chnl), rows(rs), cols(cs) {

    // Create grid in dynamic memory.
    this->grid = new core::SafeArray<BitVector *>(chnl, this->rows);

    for (size_t row = 0; row < this->rows; row++) {
        this->grid->set(row, new BitVector(chnl, this->cols));
    }
}

BitGrid::~BitGrid() {
    for (size_t row = 0; row < this->rows; row++) {
        delete this->grid->get(row);
    }

    delete this->grid;
}

bool BitGrid::operator==(const BitGrid &o) const {
    if (this->rows != o.rows) {
        return false;
    }

    for (size_t i = 0; i < this->rows; i++) {
        if (*(this->grid->get(i)) != *(o.grid->get(i))) {
            return false;
        }
    }

    return true;
}

BitVectorFileWriter::BitVectorFileWriter()
    : BitVectorFileWriter(core::CXX_DEF_CHNL) {
}

BitVectorFileWriter::BitVectorFileWriter(uint8_t chnl) 
    : FileWriter<BitVector *>(chnl) {
}


BitVectorFileWriter::~BitVectorFileWriter() {
}

bool BitVectorFileWriter::write(uint8_t handle, BitVector *element) {
    const size_t bits = element->bits;
    size_t bitsRes = ti_Write(&bits, sizeof(size_t), 1, handle);

    if (bitsRes != 1) {
        return false;
    }

    ShallowArrayFileWriter<uint8_t> *arrWriter = 
        new ShallowArrayFileWriter<uint8_t>(this->getChnl());

    bool arrRes = arrWriter->write(handle, element->vector);

    delete arrWriter; 

    return arrRes;
}

BitVectorFileReader::BitVectorFileReader()
    : BitVectorFileReader(core::CXX_DEF_CHNL) {
}

BitVectorFileReader::BitVectorFileReader(uint8_t chnl) 
    : FileReader<BitVector *>(chnl) {
}


BitVectorFileReader::~BitVectorFileReader() {
}

bool BitVectorFileReader::read(uint8_t handle, BitVector **dest) {
    size_t bits;
    size_t bitsRes = ti_Read(&bits, sizeof(size_t), 1, handle);

    if (bitsRes != 1) {
        return false;
    }

    core::SafeArray<uint8_t> *vector;

    ShallowArrayFileReader<uint8_t> *arrReader = 
        new ShallowArrayFileReader<uint8_t>(this->getChnl());

    bool vectorRes = arrReader->read(handle, &vector);

    delete arrReader;

    if (!vectorRes) {
        return false;
    }

    *dest = new BitVector(this->getChnl(), bits, vector);

    return true;
}



