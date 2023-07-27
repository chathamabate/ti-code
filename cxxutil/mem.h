#pragma once

#include <new>
#include <stdint.h>

// NOTE this is a redefinition of cutil/misc.h
// 
// Here we are using C++ new and delete keywords in tandom with a 
// memory counter.

// This should be defined if the user wants memory checking
// to take place. (Leads to bulkier and slower binary)
#define CXX_MEM_CHECKS

namespace cxxutil {
    // Memory channel compile time constants.
    constexpr uint8_t CXX_NUM_MEM_CHNLS      = 24;

    constexpr uint8_t CXX_TEST_CHNL          = 0;
    constexpr uint8_t CXX_DEF_CHNL           = 1;
    constexpr uint8_t CXX_KEY_CHNL           = 2;

    constexpr uint8_t CXX_FREE_CHNL_START    = 8;

    enum MemoryExitCode : uint8_t {
        OUT_OF_MEMORY = 0,
        OVERFLOW,
        UNDERFLOW,
        BAD_CHANNEL,
        MEMORY_LEAK,
    };

    const char *translateMEC(MemoryExitCode mec);

#ifdef CXX_MEM_CHECKS
    
    class MemoryExitRoutine;

    extern const MemoryExitRoutine * const BASIC_MER;

    void setMER(const MemoryExitRoutine *mer);

    // Memory tracker class will be hidden.
    void incrMemChnl(uint8_t memChnl);
    void decrMemChnl(uint8_t memChnl);

    void runMER(MemoryExitCode mec);

    // Will exit with error if there are memory leaks!
    void checkMemLeaks();
#endif

    // All Classes which are used to make objects in dynamic memory should
    // extend SafeObject. This class will automatically increment and decrement
    // the memory tracker without the user needing to write any special code!

    class SafeObject {
    private:
#ifdef CXX_MEM_CHECKS
        uint8_t chnl;
#endif

    public:
        SafeObject(uint8_t chnl);
        ~SafeObject();
        void *operator new(size_t size);
    };

    template <typename T> 
    class SafeArray : public SafeObject {
    private:
        uint16_t len;
        T *arr;
    public:
        SafeArray(uint8_t chnl, uint16_t len) : SafeObject(chnl) {
            this->len = len;

            T *arr = new (std::nothrow) T[len];

#ifdef CXX_MEM_CHECKS
            if (!arr) {
                runMER(MemoryExitCode::OUT_OF_MEMORY);
            } 
#endif

            this->arr = arr;
        }

        ~SafeArray() {
            delete [] this->arr;
        }

        inline uint16_t getLen() {
            return this->len;
        }

        // NOTE: No bounds checking for speed!
        inline T get(uint16_t i) {
            return this->arr[i];
        }

        inline void set(uint16_t i, T ele) {
            this->arr[i] = ele;
        }
    };
}

