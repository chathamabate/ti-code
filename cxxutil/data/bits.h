#pragma once

#include <cxxutil/core/mem.h>
#include <cxxutil/data/file.h>

namespace cxxutil { namespace data {

    typedef struct {
        size_t r;
        size_t c;
    } grid_coord_t;

    // A string of bits read left-to-right.
    class BitVector : public core::SafeObject {
        friend class BitVectorFileReader;
        friend class BitVectorFileWriter;

    private:
        const size_t bits;
        core::SafeArray<uint8_t> *vector;

        BitVector(uint8_t chnl, size_t bs, core::SafeArray<uint8_t> *v);

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

        bool operator==(const BitVector &o) const;

        inline bool operator!=(const BitVector &o) const {
            return !(*this == o);
        }
    };

    class BitVectorFileWriter : public FileWriter<BitVector *> {
    public:
        BitVectorFileWriter();    
        BitVectorFileWriter(uint8_t chnl);
        virtual ~BitVectorFileWriter();
        virtual bool write(uint8_t handle, BitVector *element) override;
    };

    class BitVectorFileReader : public FileReader<BitVector *> {
    public:
        BitVectorFileReader();    
        BitVectorFileReader(uint8_t chnl);
        virtual ~BitVectorFileReader();
        virtual bool read(uint8_t handle, BitVector **dest) override;
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

        bool operator==(const BitGrid &o) const;

        inline bool operator!=(const BitGrid &o) const {
            return !(*this == o);
        }
    };
}}
