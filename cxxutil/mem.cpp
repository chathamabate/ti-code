
#include "ti/screen.h"
#include <cxxutil/mem.h>

#include <stdlib.h>
#include <stdio.h>

#include <tice.h>

using namespace cxxutil;

MemoryTracker *MemoryTracker::singleton = nullptr;
MemoryTracker *MemoryTracker::getInstance() {
    // NOTE: This should be the only place 
    // were we dynamically create a non-safe object.
    if (!singleton) {
        singleton = new MemoryTracker(24);
    }

    return singleton;
}

MemoryTracker::MemoryTracker(uint8_t chnls) {
    this->numMemChnls = chnls;
    this->memChnls = new uint24_t[chnls];

    uint8_t i;
    for (i = 0; i < chnls; i++) {
        this->memChnls[i] = 0;
    }

    this->mer = nullptr;
}

void MemoryTracker::setMemoryExitRoutine(MemoryExitRoutine *mer) {
    this->mer = mer;
}

void MemoryTracker::runMemoryExitRoutine(MemoryExitCode mec) {
    if (this->mer) {
        this->mer->exit(this, mec);
    }

    BasicMemoryExitRoutine::getInstance()->exit(this, mec);
}

void MemoryTracker::incrMemChnl(uint8_t chnl) {
    if (chnl >= this->numMemChnls) {
        this->runMemoryExitRoutine(MemoryExitCode::BAD_CHANNEL);
        return;
    } 

    this->memChnls[chnl]++;

    if (this->memChnls[chnl] == 0) {
        this->runMemoryExitRoutine(MemoryExitCode::OVERFLOW);
    }
}

void MemoryTracker::decrMemChnl(uint8_t chnl) {
    if (chnl >= this->numMemChnls) {
        this->runMemoryExitRoutine(MemoryExitCode::BAD_CHANNEL);
        return;
    } 

    if (this->memChnls[chnl] == 0) {
        this->runMemoryExitRoutine(MemoryExitCode::UNDERFLOW);
        return;
    }

    this->memChnls[chnl]--;
}

uint24_t *MemoryTracker::getMemChnls() {
    return this->memChnls;
}

uint8_t MemoryTracker::getNumChnls() {
    return this->numMemChnls;
}

void MemoryTracker::printMemChnls() {
    char buff[20];

    uint8_t i;
    for (i = 0; i < this->numMemChnls; i++) {
        sprintf(buff, "%u: %u", i, this->memChnls[i]);
        os_PutStrFull(buff);

        if ((i + 1) % 3 == 0) {
            os_NewLine();
        } else {
            os_PutStrFull("  ");
        }
    }
}

const char *MemoryExitRoutine::getExitCodeName(MemoryExitCode mec) {
    switch (mec) {
    case MemoryExitCode::BAD_CHANNEL:
        return "Bad Channel";
    case MemoryExitCode::OVERFLOW:
        return "Overflow";
    case MemoryExitCode::UNDERFLOW:
        return "Underflow";
    case MemoryExitCode::OUT_OF_MEMORY:
        return "Out of Memory";  
    }
}

SafeObject::SafeObject(uint8_t chnl) {
#ifdef CXX_MEM_CHECKS
    this->chnl = chnl;
    MemoryTracker::getInstance()->incrMemChnl(chnl);
#else
    (void)chnl;
#endif
}

SafeObject::~SafeObject() {
#ifdef CXX_MEM_CHECKS
    MemoryTracker::getInstance()->decrMemChnl(this->chnl); 
#endif
}

void *SafeObject::operator new(size_t size) {
    void *p = malloc(size);

#ifdef CXX_MEM_CHECKS
    if (!p) {
        MemoryTracker::getInstance()
            ->runMemoryExitRoutine(MemoryExitCode::OUT_OF_MEMORY);
    }
#endif

    return p;
}

BasicMemoryExitRoutine *BasicMemoryExitRoutine::singleton = nullptr;
BasicMemoryExitRoutine *BasicMemoryExitRoutine::getInstance() {
    if (!singleton) {
        singleton = new BasicMemoryExitRoutine();
    }

    return singleton;
}

BasicMemoryExitRoutine::BasicMemoryExitRoutine() : 
    SafeObject(CXX_EXIT_ROUTINE_CHNL), MemoryExitRoutine() { }

void BasicMemoryExitRoutine::exit(MemoryTracker *mt, MemoryExitCode mec) {
    os_ClrHome();

    os_PutStrFull(MemoryExitRoutine::getExitCodeName(mec));
    os_NewLine();

    mt->printMemChnls();

    delay(3000);

    ::exit(1);
}





