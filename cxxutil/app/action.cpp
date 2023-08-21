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

Action::Action(uint24_t memChnl) 
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
    }

    return this->nextAction;
}

void LoopAction::update() {
}

void LoopAction::render() {
}

LoopAction::LoopAction(uint24_t memChnl) 
    : Action(memChnl) {
}

LoopAction::LoopAction() 
    : LoopAction(core::CXX_DEF_CHNL) {
}

LoopAction::~LoopAction() {
}
