# include "./pane.h"
#include "cxxutil/core/mem.h"

using namespace cxxutil::gui;

Pane::Pane(uint8_t memChnl) : core::SafeObject(memChnl) {
    this->inFocus = false;
}

Pane::Pane() : Pane(core::CXX_DEF_CHNL) {
}

Pane::~Pane() {
}
