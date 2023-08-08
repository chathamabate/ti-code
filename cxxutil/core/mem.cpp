
#include "ti/screen.h"
#include <cxxutil/core/mem.h>

#include <stdlib.h>
#include <stdio.h>

#include <tice.h>
#include <keypadc.h>

#include <graphx.h>

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

void BasicMemoryExitRoutine::run(MemoryTracker *mt, MemoryExitCode mec) {
    os_ClrHome();
    os_PutStrFull(translateMEC(mec));
    os_NewLine();

    mt->printMemChnls();

    waitClear();
}

BasicMemoryExitRoutine BasicMemoryExitRoutine::ONLY_VAL;

void GraphicsMemoryExitRoutine::run(MemoryTracker *mt, MemoryExitCode mec) {
    gfx_End();
    BasicMemoryExitRoutine::ONLY->run(mt, mec);
}

GraphicsMemoryExitRoutine GraphicsMemoryExitRoutine::ONLY_VAL;

#ifdef CXX_MEM_CHECKS
MemoryTracker::MemoryTracker(MemoryExitRoutine *pmer) {
    this->mer = pmer;
    // Don't think this is needed, but whatevs.
    for (int8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
        this->memChnls[i] = 0;
    }
}

void MemoryTracker::checkChnl(uint8_t memChnl) {
    if (memChnl >= CXX_NUM_MEM_CHNLS) {
        this->runMER(MemoryExitCode::BAD_CHANNEL);
    }
}

void MemoryTracker::setMER(MemoryExitRoutine *pmer) {
    this->mer = pmer;
}

// This function always exits.
void MemoryTracker::runMER(MemoryExitCode mec) {
    if (this->mer) {
        this->mer->run(this, mec);
    }

    exit(0);
}

void MemoryTracker::checkMemLeaks() {
    for (uint8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
        if (this->memChnls[i] > 0) {
            this->runMER(MemoryExitCode::MEMORY_LEAK);
        }
    }
}

bool MemoryTracker::memLeaks(uint8_t bound) {
    for (uint8_t i = bound; i < CXX_NUM_MEM_CHNLS; i++) {
        if (this->memChnls[i] > 0) {
            return true;
        }
    }

    return false;
}

void MemoryTracker::incr(uint8_t memChnl) {
    this->checkChnl(memChnl);

    this->memChnls[memChnl]++;

    if (this->memChnls[memChnl] == 0) {
        this->runMER(MemoryExitCode::OVERFLOW);
    }
}

void MemoryTracker::decr(uint8_t memChnl) {
    this->checkChnl(memChnl);

    if (this->memChnls[memChnl] == 0) {
        this->runMER(MemoryExitCode::UNDERFLOW);
    }

    this->memChnls[memChnl]--;
}

void MemoryTracker::printMemChnls() {
    constexpr uint8_t ROW_WIDTH = 4;
    char buff[20];  // 20 characters should be more than enough.

    for (int8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
        bool rowEnd = (i + 1) % ROW_WIDTH == 0;

        sprintf(buff, rowEnd ? "%u:%u" : "%u:%u  ", i, this->memChnls[i]);
        os_PutStrFull(buff);

        if (rowEnd) {
            os_NewLine();
        }
    }
}

void MemoryTracker::remember(size_t *buf) {
    for (uint8_t i = 0; i < CXX_NUM_MEM_CHNLS; i++) {
        buf[i] = this->memChnls[i];
    }
}

bool MemoryTracker::consistent(size_t *buf, uint8_t bound) {
    for (uint8_t i = bound; i < CXX_NUM_MEM_CHNLS; i++) {
        if (buf[i] != this->memChnls[i]) {
            return false;
        }
    }

    return true;
}
#else
MemoryTracker::MemoryTracker(const MemoryExitRoutine *pmer) {
    (void)pmer;
}

void MemoryTracker::checkChnl(uint8_t memChnl) {
    (void)memChnl;
}

void MemoryTracker::setMER(const MemoryExitRoutine *pmer) {
    (void)pmer;
}

void MemoryTracker::runMER(MemoryExitCode mec) {
    (void)mec;
}

void MemoryTracker::checkMemLeaks() {
}

bool MemoryTracker::memLeaks(uint8_t bound) {
    (void)bound;
    return false;
}

void MemoryTracker::incr(uint8_t memChnl) {
    (void)memChnl;
}

void MemoryTracker::decr(uint8_t memChnl) {
    (void)memChnl;
}

void MemoryTracker::printMemChnls() {
}

void MemoryTracker::remember(size_t *buf) {
    (void)buf;
}

bool MemoryTracker::consistent(size_t *buf, uint8_t bound) {
    (void)buf;
    (void)bound;
    return false;
}
#endif

MemoryTracker MemoryTracker::ONLY_VAL(core::BasicMemoryExitRoutine::ONLY);


SafeObject::SafeObject() : SafeObject(CXX_DEF_CHNL) { }

SafeObject::SafeObject(uint8_t chnl) {
#ifdef CXX_MEM_CHECKS
    this->chnl = chnl;

    // NOTE, it is assumed all safe objects are created
    // in dynamic memory. Calling the constructor will
    // always increment the memory tracker.
    MemoryTracker::ONLY->incr(chnl);
#else
    (void)chnl;
#endif
}

SafeObject::~SafeObject() {
#ifdef CXX_MEM_CHECKS
    MemoryTracker::ONLY->decr(this->chnl);
#endif
}

void *SafeObject::operator new(size_t size) {
    void *p = malloc(size);

#ifdef CXX_MEM_CHECKS
    if (!p) {
        MemoryTracker::ONLY->runMER(MemoryExitCode::OUT_OF_MEMORY);
    }
#endif

    return p;
}
