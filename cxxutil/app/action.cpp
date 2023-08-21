#include "./action.h"

#include <tice.h>
#include "cxxutil/core/mem.h"

using namespace cxxutil::app;

void Action::init() {
}

Action *Action::run() {
    return nullptr;
}

void Action::finally() {
}

Action::Action(uint8_t memChnl) 
    : core::SafeObject(memChnl) {
}

Action::Action() : Action(core::CXX_DEF_CHNL) {
}

Action::~Action() {
}

void Action::begin(Action *a) {
    // This should stop a from being deleted.
    Action *curr = a->complete();

    while (curr) {
        Action *next = curr->complete();
        delete curr;
        curr = next;
    }
}

Action *LoopAction::run() {
    while (!exitRequested) {
        this->update();

        if (this->redrawRequested) {
            this->render();
        }

        delay(this->del);
    }

    return this->nextAction;
}

void LoopAction::update() {
}

void LoopAction::render() {
}

LoopAction::LoopAction(uint24_t memChnl, uint16_t d) 
    : Action(memChnl) {
    this->redrawRequested = true; 
    this->exitRequested = false;

    this->del = d;
}

LoopAction::LoopAction(uint16_t d) 
    : LoopAction(core::CXX_DEF_CHNL, d) {
}

LoopAction::~LoopAction() {
}
