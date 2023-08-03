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

    class MemoryTracker;
    class MemoryExitRoutine;

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

    // NOTE: Memory Exit Routines should be static singletons.
    class MemoryExitRoutine {
    public:
        virtual void run(MemoryTracker *mt, MemoryExitCode mec) = 0;
    };

    class BasicMemoryExitRoutine : public MemoryExitRoutine {
    private:
        static BasicMemoryExitRoutine ONLY_VAL;
    public:
        static constexpr BasicMemoryExitRoutine *ONLY = &ONLY_VAL;
        virtual void run(MemoryTracker *mt, MemoryExitCode mec) override;
    };

    class MemoryTracker {
    private:
        static MemoryTracker ONLY_VAL;

#ifdef CXX_MEM_CHECKS
        size_t memChnls[CXX_NUM_MEM_CHNLS];
        MemoryExitRoutine *mer;
#endif

        MemoryTracker(MemoryExitRoutine *pmer);

        void checkChnl(uint8_t memChnl);
    public:
        static constexpr MemoryTracker *ONLY = &ONLY_VAL;

        // NOTE: If memchecks are off, none of these functions
        // do anything. 
        void setMER(MemoryExitRoutine *pmer);
        void runMER(MemoryExitCode mec);
        void checkMemLeaks();
        bool memLeaks(uint8_t bound);
        void incr(uint8_t memChnl);
        void decr(uint8_t memChnl);
        void printMemChnls();

        // This will copy all the current 
        // memChnl values into buf. buf thus
        // must have a size >= CXX_NUM_MEM_CHNLS.
        void remember(size_t *buf);
        
        // Returns true if the buf[i] = memChnl[i]
        // for all i >= bound.
        bool consistent(size_t *buf, uint8_t bound);
    };

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
        size_t len;
        T *arr;
    public:
        SafeArray(size_t len) : SafeArray(CXX_DEF_CHNL, len) { }

        SafeArray(uint8_t chnl, size_t len) : SafeObject(chnl) {
            this->len = len;

            T *arr = new (std::nothrow) T[len];

#ifdef CXX_MEM_CHECKS
            if (!arr) {
                MemoryTracker::ONLY->runMER(MemoryExitCode::OUT_OF_MEMORY);
            } 
#endif

            this->arr = arr;
        }

        ~SafeArray() {
            delete [] this->arr;
        }

        inline size_t getLen() const {
            return this->len;
        }

        // NOTE: No bounds checking for speed!
        inline T get(size_t i) const {
            return this->arr[i];
        }

        // getPtr and getArr do not change the safeArray
        // However, since they are used to change the safe
        // array using pointers, I will not be marking them
        // const.

        inline T *getPtr(size_t i) {
            return &(this->arr[i]);
        }

        // This kinda dangerous, so be careful!!!
        inline T *getArr() {
            return this->arr;
        }

        inline void set(size_t i, T ele) {
            this->arr[i] = ele;
        }
    };
}
}

