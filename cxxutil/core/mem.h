#pragma once

#include "ti/screen.h"
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

    // Just to help with all this syntax highlighting BS.
    typedef uint24_t U24;

    class MemoryTracker;
    class MemoryExitRoutine;

    // Simple misc macro like function which wait for the user to hit clear.
    inline void waitClear() {
        do {
            delay(100);
            kb_Scan();
        } while (!kb_IsDown(kb_KeyClear));
    }

    inline void printWaitClear(const char *msg) {
        os_PutStrFull(msg);
        os_NewLine();

        waitClear();
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

    class GraphicsMemoryExitRoutine : public MemoryExitRoutine {
    private:
        static GraphicsMemoryExitRoutine ONLY_VAL;
    public:
        static constexpr GraphicsMemoryExitRoutine *ONLY = &ONLY_VAL;
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
        MemoryExitRoutine *getMER();
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
    //
    // NOTE: that a safe object can be created in static memory if the
    // user wants to use a RAII design. This shouldn't cause any 
    // problems.

    class SafeObject {
    private:
#ifdef CXX_MEM_CHECKS
        uint8_t chnl;
#endif

    public:
        SafeObject();
        SafeObject(uint8_t chnl);

        SafeObject(const SafeObject &so);

        // NOTE: given our object is already initialized,
        // it already has a channel!
        inline void operator=(const SafeObject &so) {
            (void)so;
        }

        ~SafeObject();
        void *operator new(size_t size);

#ifdef CXX_MEM_CHECKS
        inline uint8_t getChnl() const {
            return this->chnl;
        }
#else
        constexpr uint8_t getChnl() const {
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

            // NOTE: even when len = 0, this does not return NULL.
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
        inline const T &get(size_t i) const {
            return this->arr[i];
        }

        inline const T *getArr() const {
            return this->arr;
        }

        inline const T *getPtr(size_t i) const {
            return &(this->arr[i]);
        }

        inline T *getArrMut() {
            return this->arr;
        }

        inline T *getPtrMut(size_t i) {
            return &(this->arr[i]);
        }

        inline void set(size_t i, T ele) {
            this->arr[i] = ele;
        }

        bool operator==(const SafeArray<T> &o) const {
            if (this->len != o.len) {
                return false;
            }

            if (this->arr == o.arr) {
                return true;
            }

            // Element wise check.
            for (size_t i = 0; i < this->len; i++) {
                if (this->arr[i] != o.arr[i]) {
                    return false;
                }
            }

            return true;
        }
    };

    // NOTE: this does not resize the given array.
    // It returns a copy with the larger size.
    template <typename T>
    SafeArray<T> *resize(uint8_t chnl, const SafeArray<T> *arr, size_t newLen) {
        SafeArray<T> *newArr = new SafeArray<T>(chnl, newLen); 

        size_t bound = arr->getLen();

        if (bound > newLen) {
            bound = newLen;
        }

        for (size_t i = 0; i < bound; i++) {
            newArr->set(i, arr->get(i));
        }

        return newArr;
    }
}
}

