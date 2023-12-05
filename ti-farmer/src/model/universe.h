#pragma once

#include <stdint.h>
#include <stdlib.h>

namespace tif { namespace model {

    typedef uint32_t day_count_t;

    struct universe_t;
    struct planet_t;
    struct season_t;
    struct crop_t;

    struct crop_lookup_t;

    // These structs should all be used as constants in 
    // static memory. 

    struct universe_t {
        const char *name;

        uint8_t planetsLen;
        const planet_t *planets;
    };

    // All planets have 4 seasons.
    static constexpr uint8_t NUM_SEASONS = 4;

    struct planet_t {
        // Name of this planet.
        const char *name;
        const char *description;

        // The dimmensions of mazes on this planet.
        uint8_t mazeRows;
        uint8_t mazeCols;
        
        // NOTE: Every planet will have 4 seasons.
        // 0 = Spring
        // 1 = Summer
        // 2 = Fall
        // 3 = Winter
        const season_t *seasons[NUM_SEASONS];
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
