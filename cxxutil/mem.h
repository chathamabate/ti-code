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

#ifdef CXX_MEM_CHECKS
constexpr int8_t CXX_NUM_MEM_CHNLS      = 24;

constexpr int8_t CXX_TEST_CHNL          = 0;
constexpr int8_t CXX_KEY_CHNL           = 1;

constexpr int8_t CXX_FREE_CHNL_START    = 8;
#endif

namespace cxxutil {
    #ifdef CXX_MEM_CHECKS

    enum MemoryExitCode : uint8_t {
        OUT_OF_MEMORY = 0,
        OVERFLOW,
        UNDERFLOW,
        BAD_CHANNEL,
    };

    const char *translateMEC(MemoryExitCode mec);
    
    class MemoryExitRoutine;

    extern const MemoryExitRoutine * const BASIC_MER;

    void setMER(const MemoryExitRoutine *mer);

    // Memory tracker class will be hidden.
    void incrMemChnl(uint8_t memChnl);
    void decrMemChnl(uint8_t memChnl);

    #endif
}

