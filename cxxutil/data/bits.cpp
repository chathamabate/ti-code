
#include "./bits.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil::data;

BitVector::BitVector(size_t bs) : BitVector(core::CXX_DEF_CHNL, bs) {
}

BitVector::BitVector(uint8_t chnl, size_t b) 
    : core::SafeObject(chnl), bits(b) {

    size_t len = b / 8;

    if (b % 8 > 0) {
        len++;
    }

    this->vector = new core::SafeArray<uint8_t>(chnl, len);
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


