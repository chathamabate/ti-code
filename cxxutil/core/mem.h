#pragma once

#include <new>
#include <stdint.h>

#include <keypadc.h>
#include <tice.h>


// NOTE this is a redefinition of cutil/misc.h
// 
// Here we are using C++ new and delete keywords in tandom with a 
// memory counter.

// This should be defined if the user wants memory checking
// to take place. (Leads to bulkier and slower binary)
// 
#define CXX_MEM_CHECKS

namespace cxxutil {
namespace core {

    // Simple misc macro like function which wait for the user to hit clear.
    inline void waitClear() {
        do {
            delay(100);
            kb_Scan();
        } while (!kb_IsDown(kb_KeyClear));
    }

    // Memory channel compile time constants.
    constexpr uint8_t CXX_NUM_MEM_CHNLS      = 24;

    constexpr uint8_t CXX_TEST_CHNL          = 0;

    // This is the defualt channel.
    // If a memory channel isn't given, this will be
    // used.
    constexpr uint8_t CXX_DEF_CHNL           = 1;

    // Room for cxxutil specific dynamic memory channels.

    constexpr uint8_t CXX_FREE_CHNL_START    = 8;

    enum MemoryExitCode : uint8_t {
        OUT_OF_MEMORY = 0,
        OVERFLOW,
        UNDERFLOW,
        BAD_CHANNEL,
        MEMORY_LEAK,
    };

    const char *translateMEC(MemoryExitCode mec);

    class MemoryExitRoutine;

    extern const MemoryExitRoutine * const BASIC_MER;

    // NOTE: The below functions can all be called even if memory checks
    // isn't turned on. In this case, they will do nothing!

    void setMER(const MemoryExitRoutine *mer);

    // Memory tracker class will be hidden.
    void incrMemChnl(uint8_t memChnl);
    void decrMemChnl(uint8_t memChnl);

    void runMER(MemoryExitCode mec);

    // Will exit with error if there are memory leaks!
    void checkMemLeaks();

    // this returns true if memory leaks are
    // found in any channel >= bound.
    bool memLeaks(uint8_t bound); 

    // All Classes which are used to make objects in dynamic memory should
    // extend SafeObject. This class will automatically increment and decrement
    // the memory tracker without the user needing to write any special code!

    class SafeObject {
    private:
#ifdef CXX_MEM_CHECKS
        uint8_t chnl;
#endif

    public:
        SafeObject();
        SafeObject(uint8_t chnl);

        ~SafeObject();
        void *operator new(size_t size);

#ifdef CXX_MEM_CHECKS
        inline uint8_t getChnl() {
            return this->chnl;
        }
#else
        constexpr uint8_t getChnl() {
            return 0;
        }
#endif
    };

    template <typename T> 
    class SafeArray : public SafeObject {
    private:
        uint16_t len;
        T *arr;
    public:
        SafeArray(uint16_t len) : SafeArray(CXX_DEF_CHNL, len) { }

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

        inline T *getPtr(uint16_t i) {
            return &(this->arr[i]);
        }

        // This kinda dangerous, so be careful!!!
        inline T *getArr() {
            return this->arr;
        }

        inline void set(uint16_t i, T ele) {
            this->arr[i] = ele;
        }
    };
}
}

