#pragma once

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
        virtual void exit(MemoryTracker *mt, MemoryExitCode mec) = 0;
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

    class BasicMemoryExitRoutine : SafeObject, MemoryExitRoutine {
    private:
        BasicMemoryExitRoutine();

        static BasicMemoryExitRoutine *singleton;
    public:
        static BasicMemoryExitRoutine *getInstance();
        virtual void exit(MemoryTracker *mt, MemoryExitCode mec) override;
    };
}
