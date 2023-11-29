#pragma once

#include <cxxutil/core/mem.h>

namespace cxxutil { namespace data {

    // A string of bits read left-to-right.
    class BitVector : public core::SafeObject {
    private:
        const size_t bits;
        core::SafeArray<uint8_t> *vector;

    public:
        // Requires total number of bits.
        // All bits will start as zero.
        // NOTE: bs must be at least 1.
        BitVector(size_t bs);
        BitVector(uint8_t chnl, size_t bs);

        ~BitVector();

        inline size_t getNumBits() const {
            return this->bits;
        }

        inline bool get(size_t bit) const {
            return (this->vector->get(bit / 8) >> (bit % 8)) & 0x01; 
        }

        void set(size_t bit, bool value);
    };


    class BitGrid : public core::SafeObject {
    private:
        const size_t rows;
        const size_t cols;

        core::SafeArray<BitVector *> *grid;

    public:
        BitGrid(size_t rs, size_t cs);
        BitGrid(uint8_t chnl, size_t rs, size_t cs);
        ~BitGrid();

        inline size_t getRows() const {
            return this->rows;
        }

        inline size_t getCols() const {
            return this->cols;
        }

        inline bool get(size_t r, size_t c) const {
            return this->grid->get(r)->get(c);
        }

        inline void set(size_t r, size_t c, bool value) {
            this->grid->get(r)->set(c, value);
        }
    };
}}
