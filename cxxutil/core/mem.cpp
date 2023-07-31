
#include "ti/screen.h"
#include <cxxutil/core/mem.h>

#include <stdlib.h>
#include <stdio.h>

#include <tice.h>
#include <keypadc.h>

using namespace cxxutil::core;

const char *cxxutil::core::translateMEC(MemoryExitCode mec) {
    switch (mec) {
    case MemoryExitCode::BAD_CHANNEL:
        return "Bad Channel";
    case MemoryExitCode::OUT_OF_MEMORY:
        return "Out of Memory";
    case MemoryExitCode::OVERFLOW:
        return "Overflow";
    case MemoryExitCode::UNDERFLOW:
        return "Underflow";
    case MemoryExitCode::MEMORY_LEAK:
        return "Memory Leak";
    default:
        return "Unknown Code";
    }
}

class MemoryTracker;

class cxxutil::core::MemoryExitRoutine {
public:
    virtual void run(MemoryTracker *mt, MemoryExitCode mec) const = 0;
}; 

class MemoryTracker {
private:
    uint24_t memChnls[CXX_NUM_MEM_CHNLS];
    const MemoryExitRoutine *mer;

    inline void checkChnl(uint8_t memChnl) {
        if (memChnl >= CXX_NUM_MEM_CHNLS) {
            this->runMER(MemoryExitCode::BAD_CHANNEL);
        }
    }

public:
    MemoryTracker(const MemoryExitRoutine *pmer) : mer(pmer) {
        // Don't think this is needed, but whatevs.
        for (int8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
            this->memChnls[i] = 0;
        }
    }

    void setMER(const MemoryExitRoutine *pmer) {
        this->mer = pmer;
    }

    // This function always exits.
    void runMER(MemoryExitCode mec) {
        if (this->mer) {
            this->mer->run(this, mec);
        }

        exit(0);
    }

    void checkMemLeaks() {
        for (uint8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
            if (this->memChnls[i] > 0) {
                this->runMER(MemoryExitCode::MEMORY_LEAK);
            }
        }
    }

    void incr(uint8_t memChnl) {
        this->checkChnl(memChnl);

        this->memChnls[memChnl]++;

        if (this->memChnls[memChnl] == 0){
            this->runMER(MemoryExitCode::OVERFLOW);
        }
    }

    void decr(uint8_t memChnl) {
        this->checkChnl(memChnl);

        if (this->memChnls[memChnl] == 0) {
            this->runMER(MemoryExitCode::UNDERFLOW);
        }

        this->memChnls[memChnl]--;
    }

    void printMemChnls() {
        constexpr uint8_t ROW_WIDTH = 4;
        char buff[20];  // 20 characters should be more than enough.

        for (int8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
            bool rowEnd = (i + 1) % ROW_WIDTH == 0;

            sprintf(buff, rowEnd ? "%u: %u" : "%u: %u ", i, this->memChnls[i]);
            os_PutStrFull(buff);

            if (rowEnd) {
                os_NewLine();
            }
        }
    }
};

class BasicMemoryExitRoutine : public MemoryExitRoutine {
public:
    virtual void run(MemoryTracker *mt, MemoryExitCode mec) const override {
        os_ClrHome();
        os_PutStrFull(translateMEC(mec));
        os_NewLine();

        mt->printMemChnls();

        waitClear();
    }
};

#ifdef CXX_MEM_CHECKS

static const BasicMemoryExitRoutine BASIC_MER_VAL;
const MemoryExitRoutine * const cxxutil::core::BASIC_MER = &BASIC_MER_VAL;
static MemoryTracker MT(BASIC_MER);

void cxxutil::core::incrMemChnl(uint8_t memChnl) {
    MT.incr(memChnl);
}

void cxxutil::core::decrMemChnl(uint8_t memChnl) {
    MT.decr(memChnl);
}

void cxxutil::core::setMER(const MemoryExitRoutine *mer) {
    MT.setMER(mer);
}

void cxxutil::core::runMER(MemoryExitCode mec) {
    MT.runMER(mec);
}

void cxxutil::core::checkMemLeaks() {
    MT.checkMemLeaks();
}

#else
const MemoryExitRoutine * const cxxutil::core::BASIC_MER = nullptr;

void cxxutil::core::incrMemChnl(uint8_t memChnl) {
    (void)memChnl;
}

void cxxutil::core::decrMemChnl(uint8_t memChnl) {
    (void)memChnl;
}

void cxxutil::core::setMER(const MemoryExitRoutine *mer) {
    (void)mer;
}

void cxxutil::core::runMER(MemoryExitCode mec) {
    (void)mec;
}

void cxxutil::core::checkMemLeaks() {

}
#endif

SafeObject::SafeObject() : SafeObject(CXX_DEF_CHNL) { }

SafeObject::SafeObject(uint8_t chnl) {
#ifdef CXX_MEM_CHECKS
    this->chnl = chnl;

    // NOTE, it is assumed all safe objects are created
    // in dynamic memory. Calling the constructor will
    // always increment the memory tracker.
    MT.incr(chnl);  
#else
    (void)chnl;
#endif
}

SafeObject::~SafeObject() {
#ifdef CXX_MEM_CHECKS
    MT.decr(this->chnl);
#endif
}

void *SafeObject::operator new(size_t size) {
    void *p = malloc(size);

#ifdef CXX_MEM_CHECKS
    if (!p) {
        MT.runMER(MemoryExitCode::OUT_OF_MEMORY);
    }
#endif

    return p;
}
