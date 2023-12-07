#pragma once

#include <cxxutil/core/mem.h>
#include <stdint.h>
#include <stdlib.h>

namespace tif { namespace statics {

    typedef uint32_t day_count_t;

    struct universe_t;
    struct goal_timeline_t; 
    struct planet_t;
    struct feature_t;
    struct season_t;
    struct crop_t;

    struct crop_lookup_t;

    // These structs should all be used as constants in 
    // static memory. 

    struct universe_t {
        const char *name;

        // All crops across all planets and seasons
        // follow the same timeline.
        const goal_timeline_t *goalTimeline;

        uint8_t planetsLen;
        const planet_t * const *planets;
    };

    struct goal_timeline_t {
        uint8_t timelineLen;
        cxxutil::core::U24 *timeline;
    };

    // All planets have 4 seasons.
    static constexpr uint8_t NUM_SEASONS = 4;

    struct planet_t {
        // Cost to unlock this planet.
        cxxutil::core::U24 unlockCost;

        // Name of this planet.
        const char *name;
        const char *description;

        // The dimmensions of mazes on this planet.
        uint8_t mazeRows;
        uint8_t mazeCols;

        // The dimmensions of the farm on this planet.
        uint8_t gridRows;
        uint8_t gridCols;
        
        // NOTE: Every planet will have 4 seasons.
        // 0 = Spring
        // 1 = Summer
        // 2 = Fall
        // 3 = Winter
        const season_t *seasons[NUM_SEASONS];

        // All features which can be used on this planet.
        // NOTE: features[0] should always be a plot.
        uint8_t featuresLen;
        const feature_t * const *features;
    };

    // A feature is something which can occupy a square on the farm.
    // The most notable feature is a plot.
    //
    // All other features will be cosmetic. (At least for now)
    struct feature_t {
        cxxutil::core::U24 cost;

        const char *name;
        const char *description;

        // The initial amount the user starts with.
        uint8_t initialAmt;

        // The maximum amount of this feature which can be
        // purchased.
        uint8_t maxAmt;
    }; 

    struct season_t {
        const char *description;

        // How many days the season lasts.
        day_count_t duration;

        // Crops available in this season.
        uint8_t cropsLen;
        const crop_t * const * crops;
    };

    struct crop_t {
        const char *name;
        const char *description;

        // How long before the crop is ready to harvest.
        day_count_t timeToHarvest;
    };

    // Under this look up model, there can be no more the 4 planets.
    // Also no more the 16 crops per season.

    struct crop_lookup_t {
        uint8_t planetInd : 2; 
        uint8_t seasonInd : 2;   // a few more indeces??
        uint8_t cropInd   : 4;
    };

    extern const universe_t UNIVERSE;

    // Should we provide the look up of each crops??
    // Or maybe the look up of other stuff??

}}
