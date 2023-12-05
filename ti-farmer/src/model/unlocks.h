#pragma once

#include "ti-farmer/src/model/universe.h"
#include <stdint.h>
#include <stdlib.h>

#include <cxxutil/core/mem.h>

namespace tif { namespace model {
    struct req_t;
    struct unlock_t;
    struct goal_t;
    struct goal_tree_t;

    // A requirement is an amount of a specific crop.
    struct req_t {
        crop_lookup_t crop;
        cxxutil::core::U24 amt;
    };

    // Right now, only a planet or another crop can
    // be unlocked.
    struct unlock_t {
        bool isCropUnlock;

        union {
            crop_lookup_t crop;
            size_t planetInd;    
        };
    };

    struct goal_t {
        const char *name;

        // These are the indeces of 
        // goals that must be completed before
        // this goal can be.
        size_t depsLen;
        const size_t *deps;

        // Goal specific requirements.
        size_t reqsLen;
        const goal_t *reqs;

        // Finally goal unlocks!
        size_t unlocksLen;
        const unlock_t *unlocks;
    };

    struct goal_tree_t {
        size_t goalsLen;
        const goal_t *goals;
    };

    extern const goal_tree_t GOAL_TREE;
}}
