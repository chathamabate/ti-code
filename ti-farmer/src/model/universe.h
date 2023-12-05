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

        size_t planetsLen;
        const planet_t *planets;
    };

    // Every planet has 4 seasons.
    static constexpr size_t SPRING_IND = 0;
    static constexpr size_t SUMMER_IND = 1;
    static constexpr size_t FALL_IND   = 2;
    static constexpr size_t WINTER_IND = 3;

    struct planet_t {
        // Name of this planet.
        const char *name;
        const char *description;

        // The dimmensions of mazes on this planet.
        size_t mazeRows;
        size_t mazeCols;
        
        // NOTE: Every planet will have 4 seasons.
        // 0 = Spring
        // 1 = Summer
        // 2 = Fall
        // 3 = Winter
        const season_t *seasons[4];

        size_t cropsLen;
        const crop_t *crops;
    };

    struct season_t {
        const char *description;

        // How many days the season lasts.
        day_count_t duration;
    };

    struct crop_t {
        const char *name;
        const char *description;

        // How long before the crop is ready to harvest.
        day_count_t timeToHarvest;

        // The season this crop can grow in.
        size_t seasonInd;
    };

    struct crop_lookup_t {
        size_t planetInd; 
        size_t cropInd;
    };

    extern const universe_t UNIVERSE;
}}
