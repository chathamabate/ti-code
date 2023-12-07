
#include "./bits.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil::data;

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

void BitVector::complement() {
    uint8_t *arr = this->vector->getArrMut();
    const size_t arrLen = this->vector->getLen();

    for (size_t i = 0; i < arrLen; i++) {
        arr[i] = ~arr[i];
    }
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


