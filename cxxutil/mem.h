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

#define CXX_NUM_MEM_CHNLS 32

#define CXX_TEST_CHNL           0
#define CXX_EXIT_ROUTINE_CHNL   1
#define CXX_FREE_CHNL_START     8

namespace cxxutil {
    class MemoryTracker;
    class MemoryExitRoutine;

    enum class MemoryExitCode {
        
        // A Bad channel index was given.
        BAD_CHANNEL = 0,

        // One channel has exceeded UINT24_T_MAX.
        OVERFLOW,

        // The user tried to decrement an empty channel.
        UNDERFLOW,

        // The calculator is out of memory.
        OUT_OF_MEMORY,
    };

    class MemoryTracker {
    private:
        uint24_t *memChnls;
        uint8_t numMemChnls;

        MemoryExitRoutine *mer;

        static MemoryTracker *singleton;

        MemoryTracker(uint8_t chnls);
    public:
        static MemoryTracker *getInstance();

        void setMemoryExitRoutine(MemoryExitRoutine *mer);

        // This will always exit!
        void runMemoryExitRoutine(MemoryExitCode mec);

        void incrMemChnl(uint8_t chnl);
        void decrMemChnl(uint8_t chnl);

        uint24_t *getMemChnls();
        uint8_t getNumChnls();

        void printMemChnls();
    };

    class MemoryExitRoutine {
    public: 
        static const char *getExitCodeName(MemoryExitCode mec);

        // NOTE: Dynamic Memory should never be used in this call!
        virtual void run(MemoryTracker *mt, MemoryExitCode mec) = 0;
    };

    class SafeObject {
#ifdef CXX_MEM_CHECKS
    private:
        uint8_t chnl;
#endif

    public:
        SafeObject(uint8_t ch);
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
                MemoryTracker::getInstance()
                    ->runMemoryExitRoutine(MemoryExitCode::OUT_OF_MEMORY);
            } 
#endif

            this->arr = arr;
        }

        ~SafeArray() {
            delete this->arr;
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

    class BasicMemoryExitRoutine : SafeObject, MemoryExitRoutine {
    private:
        BasicMemoryExitRoutine();

        static BasicMemoryExitRoutine *singleton;
    public:
        static MemoryExitRoutine *getInstance();
        virtual void run(MemoryTracker *mt, MemoryExitCode mec) override;
    };
}
