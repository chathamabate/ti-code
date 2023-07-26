
#include "ti/screen.h"
#include <cxxutil/mem.h>

#include <stdlib.h>
#include <stdio.h>

#include <tice.h>
#include <keypadc.h>

using namespace cxxutil;

#ifdef CXX_MEM_CHECKS

class MemoryTracker;

class cxxutil::MemoryExitRoutine {
public:
    virtual void run(MemoryTracker *mt, MemoryExitCode mec) const = 0;
}; 

class MemoryTracker {
private:
    uint24_t memChnls[CXX_NUM_MEM_CHNLS];
    const MemoryExitRoutine *mer;

    // This function always exits.
    void runMER(MemoryExitCode mec) {
        if (this->mer) {
            this->mer->run(this, mec);
        }

        exit(0);
    }

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

const char *cxxutil::translateMEC(MemoryExitCode mec) {
    switch (mec) {
    case MemoryExitCode::BAD_CHANNEL:
        return "Bad Channel";
    case MemoryExitCode::OUT_OF_MEMORY:
        return "Out of Memory";
    case MemoryExitCode::OVERFLOW:
        return "Overflow";
    case MemoryExitCode::UNDERFLOW:
        return "Underflow";
    default:
        return "Unknown Code";
    }
}

class BasicMemoryExitRoutine : public MemoryExitRoutine {
public:
    virtual void run(MemoryTracker *mt, MemoryExitCode mec) const override {
        os_ClrHome();
        os_PutStrFull(translateMEC(mec));
        os_NewLine();

        mt->printMemChnls();

        while (!kb_IsDown(kb_KeyClear)) {
            delay(50);
            kb_Scan();
        }
    }
};

static const BasicMemoryExitRoutine BASIC_MER_VAL;
const MemoryExitRoutine * const cxxutil::BASIC_MER = &BASIC_MER_VAL;

static MemoryTracker MT(cxxutil::BASIC_MER);

void cxxutil::incrMemChnl(uint8_t memChnl) {
    MT.incr(memChnl);
}

void cxxutil::decrMemChnl(uint8_t memChnl) {
    MT.decr(memChnl);
}

void cxxutil::setMER(const MemoryExitRoutine *mer) {
    MT.setMER(mer);
}

#endif
