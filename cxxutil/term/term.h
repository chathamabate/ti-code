#pragma once

#include <stdint.h>
#include <cxxutil/core/mem.h>

namespace cxxutil { namespace term {
    // The point of this will ultimately be to allow for control character
    // sequences to actually be interpreted by the terminal...
    // This will be pretty bare bones tbh, but still cool!
    class Terminal : public core::SafeObject {

    public:
        // Can we implement scrolling, is this a good or bad feature???
        // I think scrolling is good!

        // Maybe store an entire backlog of text???
        // hmmmm... scrolling could be cool...
        
        void outc() // ?
    };
}}
